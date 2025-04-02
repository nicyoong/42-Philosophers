# 42 Philosophers: Dining Philosophers Problem Solution

## Overview

The Dining Philosophers problem is a classic synchronization problem that illustrates challenges in concurrent programming. This repository contains a solution using threads and mutexes, with potential for future expansion to include process/semaphore implementations.

## Problem Description

Five philosophers sit at a round table with bowls of spaghetti. Forks are placed between each pair of adjacent philosophers. Each philosopher must eat, think, and sleep indefinitely while:

Needing two forks to eat

Not communicating with others

Avoiding starvation

Preventing deadlocks

## Technical Specifications

Mandatory Implementation (Threads/Mutexes)

Each philosopher is a thread

Each fork is protected by a mutex

Global variables forbidden

Precise timing requirements

Strict output formatting

## Future Implementation (Processes/Semaphores)

Planned extension using:

Shared memory

System V semaphores

Process isolation

Will demonstrate alternative synchronization approach