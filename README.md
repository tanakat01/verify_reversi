# verify_reversi
verify some of the results in https://github.com/eukaryo/reversi-scripts

## How to run

```
curl -O https://raw.githubusercontent.com/eukaryo/reversi-scripts/main/empty50_tasklist_edax_knowledge.csv
g++ -O3 -Wall -o main board.cc
./main
```

## What does this program prove

if the scores of 2587 positions in https://raw.githubusercontent.com/eukaryo/reversi-scripts/main/empty50_tasklist_edax_knowledge.csv are correct, the game value of the initial position of Othello game is draw (weakly solved).



