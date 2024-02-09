# Fantasy Football
Final project for the GCED, UPC course [Algorithmics and Programming 3](https://www.fib.upc.edu/en/studies/bachelors-degrees/bachelor-degree-data-science-and-engineering/curriculum/syllabus/AP3-GCED). The objective was to implement three different types of problem solving algorithms seen in class (exhaustive, greedy and metaheuristics).

The task was to fill a football team with the max possible points given a series of restrictions, how many players in each position, and budget related restricions.

## Data
We were given a `txt` file as a database. Containing one row per player, with each player following the format:
```
name;position;price;team;points
```
Check the original [here](data_base.txt).

## Usage

First compile the desired algorithm (you can use the `Makefile`). Next prepare a query for the algorithms, a file with the following format.

```
N1 N2 N3
T
J
```
Where:
* N1: amount of defenders
* N2: amount of midfielders
* N3: amount of attackers
* T: total budget
* J: budget per player

You can view `/new_benchs` and `/public_benchs` for some examples.

Next, run the algorithm by executing:

```
./<executable> <data_base> <query_file> <output_file>
```

The `run_all.sh` can be used to test the algorithms with the example queries provided.


## Algorithms
We implemented three different algorithms to tackle the problem.

### Exhaustive algorithm
Our approach was pretty simple. We saved all players in a single vector, sorted it by points and performed a basic backtracking. The moment we were not able to increase our best teams' points the algorithm stops.

### Greedy algorithm
Straightforward greedy algorithm, for each player, we sort all players and place the best possible player. Our sorting criteria gives more importance to a players points the more players we have placed.

### Metaheuristic algorithm
A GRASP based algorithm with simulated annealing and variable neighborhood search.


## Authors
Bernats Cots and Marc Franquesa


