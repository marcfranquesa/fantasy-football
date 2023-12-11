#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <fstream>
#include <cassert>
#include <vector>
#include <ctime>
#include <cmath>
using namespace std;


struct Player {
    string name;
    int price;
    int points;
    int position;
};

using VP = vector<Player>;
using VVP = vector<VP>;

using VI = vector<int>;


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


const double a = 3;
const double b = 1;

bool comp(const Player& p1, const Player& p2) {
    return pow(p1.points, a)*pow(p2.price, b) > pow(p2.points, a)*pow(p1.price, b);
}


// Reads data file containing players, saves all players into "players" as long as their price
// is less than or equal to the limit per player given
void read_players(const string& data_file, VVP& all_players, const int& limit){
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

}


void read_input(int argc, char** argv, Restrictions& restrictions, VVP& all_players){
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


void add_player(Team& team, const Player& player){
    team.P += player.points;
    team.T += player.price;
    team.players[player.position].push_back(player);
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


void greedy(const Restrictions& restrictions, const VVP& all_players,
            const string& output_file, const double& start){
    // all_players sorted wtih comp
    sort(all_players.begin(), all_players.end(), comp);

    // Initialising empty candidate team
    VVP players(4);
    Team team;
    team = {0, 0, players};
    
    for (uint i = 0; i < all_players.size(); i++){
        Player& p = all_players[i]

        if(team.T + p.price <= restrictions.T){
            if(restriccions.limits[p.pos] > int(team.players[p.pos].size())){
                add_player(team, player)
            }
        }  
    }

    write_team(team, restrictions, output_file, start);
}


int main(int argc, char** argv) {

    double start = time();
    Restrictions restrictions;
    VVP all_players;
    read_input(argc, argv, restrictions, all_players);
    const string output_file = argv[3];

    greedy(restrictions, all_players, output_file, start)
}