#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <cassert>
#include <vector>
#include <ctime>
using namespace std;


struct Player {
    string name;
    int price;
    int points;
};

using VP = vector<Player>;
using VVP = vector<VP>;

using VS = vector<string>;
using VVS = vector<VS>;

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
    // names: vector with the names of the players
    int T, P;
    VVS names;
};


double time(){
    return clock() / double(CLOCKS_PER_SEC);
}


// Reads data file containing players, saves all players into "players" as long as their price
// is less than or equal to the limit per player given
void read_players(const string& data_file, VVP& all_players, int limit){
    ifstream input(data_file);
    string bin; char cbin;
    string position_name;
    int position;
    while (not input.eof()) {
        Player player;
        auto& [name, price, points] = player;
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
            all_players[position].push_back(player);
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


void add_player(Team& team, const Player& player, int position, int index, bool remove){
    int multiplier = (remove ? -1 : 1);
    team.P += player.points * multiplier;
    team.T += player.price * multiplier;
    team.names[position][index] = player.name;
}


bool is_team_valid(const Team& team, const Restrictions& restrictions){
    return (
        (team.T <= restrictions.T)
    );
}


void write_team(const Team& team, const Restrictions& restrictions, const string& outputFile, const double& start){
    ofstream File;
    File.open(outputFile);
    File << time() - start << endl;
    VS titles = {"POR: ", "DEF: ", "MIG: ", "DAV: "};
    for (int i = 0; i < 4; ++i){
        File << titles[i] << team.names[i][0];
        for (int j = 1; j < restrictions.limits[i]; ++j) File << ';' << team.names[i][j];
        File << endl;
    }

    File << "Punts: " << team.P << endl;
    File << "Preu: " << team.T << endl;

    File.close();
}


void search(Team& team, const Restrictions& restrictions, const VVP& all_players, const string& output_file,
            const double& start, int& max_points, int position, int previous_player_index, int players_in_position){
 
    if (players_in_position == restrictions.limits[position]){
        if (position == 3) { if (team.P > max_points) {
            write_team(team, restrictions, output_file, start);
            max_points = team.P;
        }}
        else search(team, restrictions, all_players, output_file, start, max_points, position + 1, -1, 0);
        return;
    }
    int options = all_players[position].size();
    for (int i = previous_player_index + 1; 
         (i < (int) options) and (options - i >= restrictions.limits[position] - players_in_position);
         ++i){
        add_player(team, all_players[position][i], position, players_in_position, false);
        if (is_team_valid(team, restrictions)){
            search(team, restrictions, all_players, output_file, start, max_points, position, i, players_in_position + 1);
        }
        add_player(team, all_players[position][i], position, players_in_position, true);
    }
}


int main(int argc, char** argv) {

    double start = time();
    Restrictions restrictions;
    VVP all_players(4);
    read_input(argc, argv, restrictions, all_players);
    const string output_file = argv[3];

    // Initialising empty candidate team
    VVS names = {
        VS(restrictions.limits[0]),
        VS(restrictions.limits[1]),
        VS(restrictions.limits[2]),
        VS(restrictions.limits[3])
    };
    Team team = {0, 0, names};

    int max_points = 0;
    search(team, restrictions, all_players, output_file, start, max_points, 0, -1, 0);
}