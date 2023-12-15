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

using VI = vector<int>;


struct Player {
    string name;
    int price;
    int points;
    int position;
};

using VP = vector<Player>;
using VVP = vector<VP>;


// Used to define the restrictions
struct Restrictions {
    // limits:
    // 0: required goalkeepers
    // 1: required defenders
    // 2: required midfielders
    // 3: required attackers
    VI limits;
    // T: total limit
    // J: limit per player
    int T, J;
};


// Used to define our current team
struct Team {
    // T: current cost
    // P: current points
    // players: vector with the players
    int T, P;
    VVP players;
};


double time(){
    return clock() / CLOCKS_PER_SEC;
}


bool comp(const Player& a, const Player& b) {
    const double c1 = 4;
    const double c2 = 1;
    return (pow(a.points, c1) / pow(a.price, c2)) > (pow(b.points, c1) / pow(b.price, c2));
}


// Reads data file containing players, saves all players into "all_players" as long as their price
// is less than or equal to the limit per player given
void read_players(const string& data_file, VP& all_players, const int& limit){
    ifstream input(data_file);
    string bin; char cbin;
    string position_name;
    int position;
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

    sort(all_players.begin(), all_players.end(), comp);
}


void read_input(int argc, char** argv, Restrictions& restrictions, VP& all_players){
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


void write_team(const Team& team, const Restrictions& restrictions, const string& outputFile, const double& start){
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



// to select a random player uniformly in the CL with position pos
Player select_random_player(VP& candidate_list, int& alpha){
    Player p;
    int rnd = rand() % alpha;
    p = candidate_list[rnd];
    // delete rnd player from the candidate list
    alpha = alpha - 1;
    return p;
}

bool full(const vector<int>& size_pos, const Restrictions& restrictions){
    for(int k = 0; k < 4; k++){
        if(size_pos[k] != restrictions[k]){
            return false;
        }
    }
    return true;
}   


// generates a random_team
Team greedy_radomized_team(const VP& all_players, const Restrictions& restrictions, const int& alpha){

    // Initialising empty team
    VVP players = {
        VP(restrictions.limits[0]),
        VP(restrictions.limits[1]),
        VP(restrictions.limits[2]),
        VP(restrictions.limits[3])
    };
    Team team;
    team = {0, 0, players};
    vector<int> size_pos = {0,0,0,0};

    while(alpha > 0 and !full(team, restrictions)){
        Player& p = select_random_player(all_players, alpha);
        if(team.T + p.price <= restrictions.T and (restrictions.limits[p.position] > size_pos[p.position])){
            add_player(team, p, size_pos[p.position]);
        }  
    }

    if (not full(team, restrictions)){
        // add fake players to team
    }

    return team;
}


// find the best team around team
// searching players that fit the team that increases the team's points
Team local_search(Team team, const VVP& all_players, const Restrictions& restrictions){

}


void metaheuristic(const Restrictions& restrictions, VVP& all_players,
            const string& output_file, const double& start){
    Team team, best_team;
    vector<int> alpha;

    best_team = greedy_randomized_team(all_players, restrictions, alpha);
    while true{
        team = local_search(best_team, all_players, restrictions);

        if (team.P > best_team.P) best_team = team;
    }

    write_team(best_team, restrictions, output_file, start);
}


int main(int argc, char** argv) {

    double start = time();
    Restrictions restrictions;
    VVP all_players;
    read_input(argc, argv, restrictions, all_players);
    const string output_file = argv[3];

    metaheuristic(restrictions, all_players, output_file, start);
}