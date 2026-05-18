## 2024-05-18 - Repetitive strlen calls in models.c
**Learning:** The models.c file contains redundant O(N) strlen calls within parsing loops.
**Action:** Cache strlen results into local variables when repeatedly evaluating length in sequential logic paths.
