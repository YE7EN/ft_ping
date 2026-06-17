#!/bin/bash

# ============================================================
#  Batterie de tests ft_ping sous valgrind
#  Usage : sudo ./test_ft_ping.sh
#  (sudo nécessaire pour les raw sockets ICMP)
# ============================================================

# --- Configuration ---
PING=./ft_ping                      # chemin de ton binaire
REF=~/ref/inetutils-2.0/ping/ping   # binaire de référence (pour comparaison manuelle)
TARGET=8.8.8.8                      # cible qui répond
DEAD=192.0.2.1                      # cible non routable (timeout)
BADHOST=nimportequoi.invalid        # host qui ne se résout pas
LANHOST=10.0.2.99                   # IP LAN inexistante (erreur ICMP) - adapte si besoin

VG="valgrind --leak-check=full --show-leak-kinds=all --error-exitcode=42"

# --- Couleurs ---
GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m'

PASS=0
FAIL=0

# ============================================================
#  Fonction de test
#  $1 = description
#  $2 = arguments passés à ft_ping
#  $3 = code de sortie attendu (optionnel, "any" pour ignorer)
#  $4 = timeout en secondes (pour les pings infinis, optionnel)
# ============================================================
run_test() {
    local desc="$1"
    local args="$2"
    local expected_code="${3:-any}"
    local tmout="${4:-0}"

    echo -e "${BLUE}=== TEST: ${desc} ===${NC}"
    echo -e "${YELLOW}  cmd: $PING $args${NC}"

    local out
    local code
    if [ "$tmout" -gt 0 ]; then
        # ping infini : on coupe au bout de tmout secondes avec SIGINT
        out=$(timeout --signal=INT "$tmout" $VG $PING $args 2>&1)
        code=$?
    else
        out=$($VG $PING $args 2>&1)
        code=$?
    fi

    # valgrind renvoie 42 si erreur mémoire (grâce à --error-exitcode=42)
    local mem_ok=1
    if echo "$out" | grep -q "ERROR SUMMARY: 0 errors"; then
        mem_ok=1
    else
        mem_ok=0
    fi

    # vérification fuites définitives
    local leak_ok=1
    if echo "$out" | grep -q "definitely lost: 0 bytes"; then
        leak_ok=1
    elif echo "$out" | grep -q "All heap blocks were freed"; then
        leak_ok=1
    else
        leak_ok=0
    fi

    # affichage du verdict mémoire
    if [ "$mem_ok" -eq 1 ] && [ "$leak_ok" -eq 1 ]; then
        echo -e "  ${GREEN}[MEM OK]${NC} pas d'erreur valgrind, pas de fuite definitive"
        PASS=$((PASS+1))
    else
        echo -e "  ${RED}[MEM FAIL]${NC} erreur valgrind ou fuite detectee !"
        echo "$out" | grep -E "ERROR SUMMARY|definitely lost|Invalid|uninitialised" | head -5
        FAIL=$((FAIL+1))
    fi

    # vérification du code de sortie si demandé
    if [ "$expected_code" != "any" ]; then
        if [ "$code" -eq "$expected_code" ]; then
            echo -e "  ${GREEN}[CODE OK]${NC} code de sortie = $code (attendu $expected_code)"
        else
            echo -e "  ${RED}[CODE FAIL]${NC} code de sortie = $code (attendu $expected_code)"
        fi
    fi

    echo ""
}

# ============================================================
#  Vérifications préliminaires
# ============================================================
if [ ! -f "$PING" ]; then
    echo -e "${RED}Erreur : $PING introuvable. Lance ce script depuis le dossier du projet (avec make fait).${NC}"
    exit 1
fi

if [ "$EUID" -ne 0 ]; then
    echo -e "${RED}Erreur : ce script doit etre lance avec sudo (raw sockets).${NC}"
    exit 1
fi

echo -e "${BLUE}############################################${NC}"
echo -e "${BLUE}#   BATTERIE DE TESTS ft_ping (valgrind)   #${NC}"
echo -e "${BLUE}############################################${NC}"
echo ""

# ============================================================
#  1. CAS D'ERREUR DE PARSING (sortie immediate)
# ============================================================
echo -e "${YELLOW}--- Groupe 1 : parsing & arguments ---${NC}\n"

run_test "Aucun argument (missing host operand)"      ""                  64
run_test "Aide -?"                                    "-?"                0
run_test "Option inconnue -z"                         "-z $TARGET"        64
run_test "Argument manquant -c"                       "-c"                64
run_test "Argument manquant -s"                       "-s"                64
run_test "Valeur non numerique -c abc"                "-c abc $TARGET"    1
run_test "Valeur -s trop grande (65400)"              "-s 65400 $TARGET"  1
run_test "Valeur -s enorme (overflow)"                "-s 999999999999999999999 $TARGET" 1
run_test "Valeur -w overflow (INT_MAX+1)"             "-w 2147483648 $TARGET" 1

# ============================================================
#  2. RESOLUTION & SETUP
# ============================================================
echo -e "${YELLOW}--- Groupe 2 : resolution & setup ---${NC}\n"

run_test "Host invalide (unknown host)"               "$BADHOST"          1
run_test "Resolution hostname (google.com)"           "-c 1 google.com"   0

# ============================================================
#  3. PING NORMAL
# ============================================================
echo -e "${YELLOW}--- Groupe 3 : ping fonctionnel ---${NC}\n"

run_test "Ping simple -c 2"                           "-c 2 $TARGET"      0
run_test "Ping verbose -v -c 2"                       "-v -c 2 $TARGET"   0
run_test "Ping numeric -n -c 2"                       "-n -c 2 $TARGET"   0

# ============================================================
#  4. BONUS -s (tailles)
# ============================================================
echo -e "${YELLOW}--- Groupe 4 : bonus -s (taille payload) ---${NC}\n"

run_test "Taille -s 0 (sans timestamp)"              "-s 0 -c 1 $TARGET"    0
run_test "Taille -s 8 (trop petit pour timeval)"     "-s 8 -c 1 $TARGET"    0
run_test "Taille -s 20"                              "-s 20 -c 1 $TARGET"   0
run_test "Taille -s 100"                             "-s 100 -c 1 $TARGET"  0
run_test "Taille -s 65399 (max accepte)"            "-s 65399 -c 1 $TARGET" any

# ============================================================
#  5. BONUS -W / -w (timeouts)
# ============================================================
echo -e "${YELLOW}--- Groupe 5 : bonus -W / -w (timeouts) ---${NC}\n"

run_test "Linger -W 1 sur cible morte"               "-W 1 -c 1 $DEAD"      0
run_test "Timeout global -w 2"                       "-w 2 $TARGET"         0
run_test "Combinaison -w 2 -c 10"                    "-w 2 -c 10 $TARGET"   0

# ============================================================
#  6. CAS DE PERTE & ERREURS ICMP
# ============================================================
echo -e "${YELLOW}--- Groupe 6 : pertes & erreurs ICMP ---${NC}\n"

run_test "Cible non routable (100% loss)"            "-c 2 $DEAD"           0
run_test "Erreur ICMP verbose (LAN inexistant)"      "-v -c 2 $LANHOST"     any
run_test "Erreur ICMP verbose + numeric"             "-v -n -c 2 $LANHOST"  any

# ============================================================
#  7. PING INFINI + CTRL-C (simule via timeout SIGINT)
# ============================================================
echo -e "${YELLOW}--- Groupe 7 : ping infini + interruption ---${NC}\n"

run_test "Ping infini interrompu apres 3s"           "$TARGET"              any  3

# ============================================================
#  RESUME
# ============================================================
echo -e "${BLUE}############################################${NC}"
echo -e "${BLUE}#                 RESUME                   #${NC}"
echo -e "${BLUE}############################################${NC}"
echo -e "  ${GREEN}Tests memoire OK : $PASS${NC}"
echo -e "  ${RED}Tests memoire FAIL : $FAIL${NC}"
echo ""

if [ "$FAIL" -eq 0 ]; then
    echo -e "${GREEN}>>> Tous les tests memoire sont passes ! <<<${NC}"
    exit 0
else
    echo -e "${RED}>>> $FAIL test(s) ont echoue, verifie la sortie ci-dessus. <<<${NC}"
    exit 1
fi
