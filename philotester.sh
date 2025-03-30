#!/bin/bash
# File: philotester.sh

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
NC='\033[0m' # No Color

# Path to the philo executable
PHILO=./philo

# Test Helper Functions
function log_test() {
    echo -e "\n=== Test: $1 ==="
}

function run_philo() {
    "$PHILO" "$@" 2>&1
}

function assert_contains() {
    local output="$1"
    local expected="$2"
    if echo "$output" | grep -q "$expected"; then
        echo -e "${GREEN}PASS: Found '$expected'${NC}"
        return 0
    else
        echo -e "${RED}FAIL: Did not find '$expected'${NC}"
        return 1
    fi
}

function assert_not_contains() {
    local output="$1"
    local unexpected="$2"
    if echo "$output" | grep -qv "$unexpected"; then
        echo -e "${GREEN}PASS: No '$unexpected' found${NC}"
        return 0
    else
        echo -e "${RED}FAIL: Found '$unexpected'${NC}"
        return 1
    fi
}

function assert_timing() {
    local output="$1"
    local min_time="$2"
    local max_time="$3"
    local death_time=$(echo "$output" | grep "died" | awk '{print $1}')
    
    if [ -z "$death_time" ]; then
        echo -e "${RED}FAIL: No death detected${NC}"
        return 1
    fi
    
    if [ "$death_time" -ge "$min_time" ] && [ "$death_time" -le "$max_time" ]; then
        echo -e "${GREEN}PASS: Died at ${death_time}ms (expected ${min_time}-${max_time}ms)${NC}"
        return 0
    else
        echo -e "${RED}FAIL: Died at ${death_time}ms (expected ${min_time}-${max_time}ms)${NC}"
        return 1
    fi
}

# Test Cases
function test_invalid_arguments() {
    log_test "Invalid arguments"
    
    echo "Case 1: Negative numbers"
    output=$(run_philo -1 800 200 200)
    assert_contains "$output" "Invalid arguments"
    
    echo "Case 2: Non-integer args"
    output=$(run_philo 5 abc 200 200)
    assert_contains "$output" "Invalid arguments"
    
    echo "Case 3: Missing args"
    output=$(run_philo 5 800 200)
    assert_contains "$output" "Usage"
}

function test_single_philosopher() {
    log_test "Single philosopher dies"
    output=$(run_philo 1 800 200 200)
    assert_contains "$output" "died"
    assert_contains "$output" "1 died"
}

function test_no_deaths() {
    log_test "No deaths under normal conditions (5s timeout)"
    output=$(timeout 5s run_philo 5 800 200 200)
    assert_not_contains "$output" "died"
}

function test_single_meal_requirement() {
    log_test "Single meal requirement"
    output=$(run_philo 3 800 200 200 1)
    meal_counts=$(echo "$output" | grep "is eating" | awk '{print $2}' | sort | uniq -c)
    if [ $(echo "$meal_counts" | awk '{if ($1 < 1) print "FAIL"}') ]; then
        echo -e "${RED}FAIL${NC}"
    else
        echo -e "${GREEN}PASS${NC}"
    fi
}

function test_optional_meals() {
    log_test "Stop after required meals (3 meals)"
    output=$(run_philo 5 800 200 200 3)
    
    # Check all philosophers ate at least 3 times
    local all_ok=true
    for ((i=1; i<=5; i++)); do
        count=$(echo "$output" | grep -c "$i is eating")
        if [ "$count" -lt 3 ]; then
            echo -e "${RED}FAIL: Philosopher $i ate only $count times${NC}"
            all_ok=false
        fi
    done
    
    if $all_ok; then
        echo -e "${GREEN}PASS: All philosophers ate at least 3 times${NC}"
    fi
}

function test_death_timing() {
    start_time=$(date +%s%3N)  # Get current timestamp in ms
    
    output=$(run_philo 2 310 200 100)
    
    # Extract death time from logs
    death_line=$(echo "$output" | grep "died")
    death_timestamp=$(echo "$death_line" | awk '{print $1}')
    death_philo=$(echo "$death_line" | awk '{print $2}')
    
    # Calculate elapsed time in ms
    elapsed_ms=$((death_timestamp - start_time))
    
    if [ -z "$death_timestamp" ]; then
        echo -e "${RED}FAIL: No death detected${NC}"
        return
    fi
    
    if [ $elapsed_ms -ge 300 ] && [ $elapsed_ms -le 320 ]; then
        echo -e "${GREEN}PASS: Died after ${elapsed_ms}ms (expected 310±10ms)${NC}"
    else
        echo -e "${RED}FAIL: Died after ${elapsed_ms}ms (expected 310±10ms)${NC}"
    fi
}

function test_large_number() {
    log_test "Large number of philosophers (200, 10s timeout)"
    output=$(timeout 10s run_philo 200 800 200 200)
    assert_not_contains "$output" "died"
}

# Verify fork pickup order doesn't cause deadlocks
function test_fork_order() {
    log_test "Fork order deadlock check (50ms)"
    # Should complete without deadlock
    output=$(timeout 1s run_philo 4 50 20 20)
    assert_not_contains "$output" "died"
}

function test_log_format() {
    log_test "Log format validation"
    output=$(run_philo 2 100 50 50)
    
    # Check all lines match expected patterns
    valid_lines=$(echo "$output" | grep -E "^[0-9]+ [1-2] (has taken a fork|is eating|is sleeping|is thinking|died)$" | wc -l)
    total_lines=$(echo "$output" | wc -l)
    
    if [ "$valid_lines" -eq "$total_lines" ]; then
        echo -e "${GREEN}PASS: All log lines formatted correctly${NC}"
    else
        echo -e "${RED}FAIL: $((total_lines-valid_lines)) malformed log lines${NC}"
    fi
}

function test_death_precision() {
    log_test "Death timing precision (200ms ±10ms)"
    
    # Run the program and capture output with timestamps
    output=$( { time -p ./philo 3 200 100 100; } 2>&1 )
    
    # Extract the death line
    death_line=$(echo "$output" | grep "died")
    
    if [ -z "$death_line" ]; then
        echo -e "${RED}FAIL: No death detected${NC}"
        return 1
    fi
    
    # Get the death timestamp (absolute time in ms)
    death_time=$(echo "$death_line" | awk '{print $1}')
    
    # Get the first timestamp (program start time)
    first_line=$(echo "$output" | head -1)
    start_time=$(echo "$first_line" | awk '{print $1}')
    
    # Calculate elapsed time in ms
    elapsed_ms=$((death_time - start_time))
    
    # Verify timing (200ms ±10ms)
    if [ $elapsed_ms -ge 190 ] && [ $elapsed_ms -le 210 ]; then
        echo -e "${GREEN}PASS: Died after ${elapsed_ms}ms${NC}"
        return 0
    else
        echo -e "${RED}FAIL: Died after ${elapsed_ms}ms (expected 200±10ms)${NC}"
        return 1
    fi
}

# Main execution
echo "Starting Philosophers Test Suite"
echo "==============================="

test_invalid_arguments
test_single_philosopher
test_no_deaths
test_single_meal_requirement
test_optional_meals
test_death_timing
test_large_number
test_fork_order
test_log_format
test_death_precision


echo -e "\nTest Suite Complete"