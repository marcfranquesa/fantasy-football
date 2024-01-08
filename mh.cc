#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <fstream>
#include <cassert>
#include <vector>
#include <ctime>
#include <cmath>
#include <random>
#include <cstdlib>
using namespace std;


struct Player {
    string name;
    int price;
    int points;
    int position;
};

bool operator==(const Player& a, const Player& b){
    return 
    (
        b.name == a.name and
        b.price == a.price and
        b.points == a.points and
        b.position == a.position
    );
}

using Players = vector<Player>;
using PlayersByPosition = vector<Players>;


/*
    Used to define the restrictions
        limits:
            pos 0: required goalkeepers
            pos 1: required defenders
            pos 2: required midfielders
            pos 3: required attackers
        T: total limit
        J: limit per player
*/
struct Restrictions {
    vector<int> limits;
    int T, J;
};


/*
    Used to define our current team
        T: current cost
        P: current points
        players: vector with the players

        isOnTeam: indicates if player is on team
*/
struct Team {
    int T, P;
    PlayersByPosition players;

    bool isOnTeam(const Player& player2) const {
        for (const Players& playerVector : players) {
            for (const Player& player : playerVector) {
                if (player == player2) {
                    return true;
                }
            }
        }
        return false;
    }
};


// Returns processor time consumed by the program in seconds
double time(){
    return (double) clock() / CLOCKS_PER_SEC;
}


// Defined as a class to be able to initialise it with parameters
class player_sorter {
    double c1, c2;
    public:
        player_sorter(double c1, double c2) : c1(c1), c2(c2) {}

        double value(const Player& p) const {
            return pow(p.points, c1) / pow(p.price, c2);
        }

        bool operator()(const Player& p1, const Player& p2) const {
            if (p1.points == 0 and not (p2.points == 0)) return false;
            if (p2.points == 0 and not (p1.points == 0)) return true;
            return value(p1) > value(p2);
        }
};


/*
    Reads data file containing players, saves all players into "all_players" as long as their price
    is less than or equal to the limit per player given
*/
void read_players(const string& data_file, Players& all_players, const int& limit){
    ifstream input(data_file);
    string bin; char cbin;
    string position_name;
    while (not input.eof()) {
        Player player;
        auto& [name, price, points, position] = player;
        getline(input, name, ';');    if (name == "") break;
        getline(input, position_name, ';');
        input >> price;
        input >> cbin;
        getline(input, bin, ';');
        input >> points;
        getline(input, bin);
        // Ignores players more expensive than limit
        if (price <= limit) {
            if (position_name == "por") position = 0;
            else if (position_name == "def") position = 1;
            else if (position_name == "mig") position = 2;
            else if (position_name == "dav") position = 3;
            else throw invalid_argument("Invalid position");
            all_players.push_back(player);
        }
    }
    input.close();
}


void read_input(int argc, char** argv, Restrictions& restrictions, Players& all_players){
    if (argc != 4) {
        cout << "Incorrect Syntax" << endl;
        cout << "    Syntax:  " << argv[0] << " <data_file> <input_file> <output_file>" << endl;
        cout << "    Example: " << argv[0] << " data_base.txt input.txt output.txt" << endl;
        exit(1);
    }
    int N1, N2, N3;
    auto& [limits, T, J] = restrictions;
    ifstream input(argv[2]);
    input >> N1 >> N2 >> N3 >> T >> J;
    limits = {1, N1, N2, N3};
    input.close();

    read_players(argv[1], all_players, J);
}


void add_player(Team& team, const Player& player, int& index){
    team.P += player.points;
    team.T += player.price;
    team.players[player.position][index] = player;
    index += 1;
}


void write_team(
    const Team& team, const Restrictions& restrictions, const string& outputFile, const double& start
){
    ofstream File;
    File.open(outputFile);
    File << fixed << setprecision(1) << time() - start << endl;
    vector<string> titles = {"POR: ", "DEF: ", "MIG: ", "DAV: "};
    for (int i = 0; i < 4; ++i){
        File << titles[i] << team.players[i][0].name;
        for (int j = 1; j < restrictions.limits[i]; ++j) File << ';' << team.players[i][j].name;
        File << endl;
    }

    File << "Punts: " << team.P << endl;
    File << "Preu: " << team.T << endl;

    File.close();
}


double generate_random_number(double lower, double upper){
    double random = (double) rand() / RAND_MAX;
    return lower + random * (upper - lower);
}


// Checks if a player can be added to the team based on the restrictions.
bool is_player_valid(
    const Team& team, const Restrictions& restrictions, const Player& p, const vector<int>& size_pos
){
    return (
        p.price + team.T <= restrictions.T and
        p.price <= restrictions.J and
        size_pos[p.position] < restrictions.limits[p.position] and
        not team.isOnTeam(p)
    );
}


// Randomly select a player from the Candidate List with the position 'pos' using a uniform distribution
Player select_random_player(Players& candidate_list){

    int rnd = rand() % candidate_list.size();

    return candidate_list[rnd];
}

// Generates a random_team
Team greedy_randomized_team(Players& all_players, const Restrictions& restrictions, const int& alpha){
    
    // Exponent of the points of a player (used in sorting)
    double c1 = generate_random_number(3, 8);
    // Exponent of the remaining money after using player (used in sorting)
    double c2 = generate_random_number(1, 6);

    sort(all_players.begin(), all_players.end(), player_sorter(c1, c2));

    // Initialising empty team
    PlayersByPosition players = {
        Players(restrictions.limits[0]),
        Players(restrictions.limits[1]),
        Players(restrictions.limits[2]),
        Players(restrictions.limits[3])
    };

    Team team = {0, 0, players};
    vector<int> size_pos = {0, 0, 0, 0};
    Players CL;
    int k; // iterator of the CL
    int j; // iterator of all_players

    for (int total_players = 0; total_players < 11; ++total_players){
        // Create a candidate list and select a random player
        CL = Players();

        // Generate a candidate list with the first alpha available players from 'all_players' (already sorted)
        // We introduced the second condition in the event that there are no alpha players available to add.
        for (j = k = 0; k < alpha and j < all_players.size(); ++j){
            Player p = all_players[j];
            if (is_player_valid(team, restrictions, p, size_pos)){
                CL.push_back(p);
                k++;
            }
        }

        Player p = select_random_player(CL);
        add_player(team, p, size_pos[p.position]);
    }

    return team;
}


// Returns a random neighbour team
Team find_random_neighbour(
    Team team, const Players& all_players, const Restrictions& restrictions
){

    bool found = false;
    while (not found){
        // find a random player
        int index_rnd = rand() % all_players.size();
        Player p = all_players[index_rnd];
        if(not team.isOnTeam(p)){
            // exchange it with a player in team with same position chosen randomly
            int index_deleted = rand() % restrictions.limits[p.position];
            Player p_deleted = team.players[p.position][index_deleted];

            if(team.T + p.price - p_deleted.price < restrictions.T){
                team.T -= p_deleted.price;
                team.P -= p_deleted.points;
                add_player(team, p, index_deleted);
                
                found = true;
            }
        }
    }

    return team;
}


// Defines the probability of accepting a worsening move using Boltzman probability
double boltzman_probability(const int& punts1, const int& punts2, const int& T){
    return exp((double) (punts1 - punts2) / T);
}


/* 
    Applies a local search around team
    Using Simulated Annnealing, allowing teams with less points
    After 50 steps without any improvement we move on to the next random team
*/
Team local_search(
    const Team& team, const Players& all_players, const Restrictions& restrictions
){
    
    Team local_best = team;
    Team actual = team;
    int k = 0; // counts the number of steps without any improvement 
    int T = 1e5; // Boltzman temperature
    const double a = 0.7; // geometric constant

    while (k < 50){
        Team t = find_random_neighbour(actual, all_players, restrictions);

        if (t.P > actual.P){
            actual = t;
            if (t.P > local_best.P){
                local_best = t;
                k = 0;
            }
        }
        else{
            double random = generate_random_number(0, 1);
            double prob = boltzman_probability(t.P, actual.P, T);
            if (random <= prob){
                actual = t;
            }
            k++;
            T *= a;
        }
        
    }
    
    return local_best;

}


void metaheuristic(
    const Restrictions& restrictions, Players& all_players,
    const string& output_file, const double& start
){
    Team team, best_team;
    const int alpha = 5; // CL size
    best_team.P = 0;

    for(int k = 0; k < 5e4; ++k){
        team = greedy_randomized_team(all_players, restrictions, alpha);
        team = local_search(team, all_players, restrictions);

        if (team.P > best_team.P){
            best_team = team;
            write_team(best_team, restrictions, output_file, start);
        }
    }
}


int main(int argc, char** argv) {

    const double start = time();
    Restrictions restrictions;
    Players all_players;
    read_input(argc, argv, restrictions, all_players);
    const string output_file = argv[3];

    metaheuristic(restrictions, all_players, output_file, start);
}