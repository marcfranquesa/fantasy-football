#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <cassert>
#include <vector>
#include <map>
using namespace std;


struct Player {
    string name;
    int price;
    int points;
};

using VP = vector<Player>;
using VVP = vector<VP>;

using VS = vector<string>;


// Used to define the restrictions
struct Restrictions {
    // N0: required goalkeepers
    // N1: required defenders
    // N2: required midfielders
    // N3: required attackers
    // T: total limit
    // J: limit per player
    static const int N0 = 1;
    int N1, N2, N3, T, J;
};


// Used to define our current team
struct Team {
    // T: current cost
    // P: current points
    // names: vector with the names of the players
    int T, P;
    VS names;
};


const unordered_map<string, int> POSITION_TRANSLATOR = {
    {"por", 0},
    {"def", 1},
    {"mig", 2},
    {"dav", 3},
};


// Reads data file containing players, saves all players into "players" as long as their price
// is less than or equal to the limit per player given
void read_players(const string& data_file, VVP& all_players, int limit){
    ifstream input(data_file);
    string bin; char cbin;
    string position;
    while (not input.eof()) {
        Player player;
        auto& [name, price, points] = player;
        getline(input, name, ';');    if (name == "") break;
        getline(input, position, ';');
        input >> price;
        input >> cbin;
        getline(input, bin, ';');
        input >> points;
        getline(input, bin);
        if (price <= limit) all_players[POSITION_TRANSLATOR.at(position)].push_back(player);
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
    auto& [N1, N2, N3, T, J] = restrictions;
    ifstream input(argv[2]);
    input >> N1 >> N2 >> N3 >> T >> J;
    input.close();

    read_players(argv[1], all_players, J);
}


int get_position(const Team& team, const Restrictions& restrictions, int current_player){
    if (current_player >= restrictions.N2 + restrictions.N1 + restrictions.N0) return 3;
    if (current_player >= restrictions.N1 + restrictions.N0) return 2;
    if (current_player >= restrictions.N0) return 1;
    if (current_player == 0) return 0;

    throw invalid_argument("invalid current_player");
}


// Resets previous player index if needed
void reset_previous_player_index(const Restrictions& restrictions, int& previous_player_index, int current_players){
    if ((current_players == 0) or
        (current_players == restrictions.N0) or
        (current_players == restrictions.N0 + restrictions.N1) or
        (current_players == restrictions.N0 + restrictions.N1 + restrictions.N2)
    )
        previous_player_index = -1;
}


void add_player(Team& team, const Player& player, int index, bool remove){
    
    int multiplier = (remove ? -1 : 1);
    team.P += player.points * multiplier;
    team.T += player.price * multiplier;
    team.names[index] = player.name;
}


bool is_team_valid(const Team& team, const Restrictions& restrictions, const VVP& all_players, int max_points,
                   int position, int current_player_index, int current_players){
    return (
        (team.T <= restrictions.T) and 
        ((team.P + (11 - current_players) * restrictions.J) > max_points)
    );
}


void write_team(const Team& team, const Restrictions& restrictions, const string& outputFile){
    /*
    finishTime = std::clock();
    duration = ( std::clock() - start ) / (double) CLOCKS_PER_SEC;
    */
    ofstream File;
    File.open(outputFile);

    int base = 0;
    File << "POR: " << team.names[base];
    for (int i = base + 1; i < restrictions.N0; ++i) File << ';' << team.names[i]; File << endl;
    base += restrictions.N0;

    File << "DEF: " << team.names[base];
    for (int i = base + 1; i < base + restrictions.N1; ++i) File << ';' << team.names[i]; File << endl;
    base += restrictions.N1;

    File << "MIG: " << team.names[base];
    for (int i = base + 1; i < base + restrictions.N2; ++i) File << ';' << team.names[i]; File << endl;
    base += restrictions.N2;

    File << "DAV: " << team.names[base];
    for (int i = base + 1; i < base + restrictions.N3; ++i) File << ';' << team.names[i]; File << endl;
    base += restrictions.N3;

    File << "Punts: " << team.P << endl;
    File << "Preu: " << team.T << endl;

    File.close();
}


void search(Team& team, const Restrictions& restrictions, const VVP& all_players, const string& output_file, 
            int& max_points, int previous_player_index, int current_players){
    
    cout << current_players << endl;
    if (current_players == 11){
        if (team.P > max_points) {
            write_team(team, restrictions, output_file);
            max_points = team.P;
        }
        return;
    }
    int position = get_position(team, restrictions, current_players);
    reset_previous_player_index(restrictions, previous_player_index, current_players);

    for (int i = previous_player_index + 1; i < (int) all_players[position].size(); ++i){
        add_player(team, all_players[position][i], current_players, false);
        if (is_team_valid(team, restrictions, all_players, max_points, position, i, current_players + 1)){
            search(team, restrictions, all_players, output_file, max_points, i, current_players + 1);
        }
        add_player(team, all_players[position][i], current_players, true);
    }
}


int main(int argc, char** argv) {
    Restrictions restrictions;
    VVP all_players(4);
    read_input(argc, argv, restrictions, all_players);
    const string output_file = argv[3];
    for (VP aa : all_players) cout << aa.size() << endl;

    Team team = {0, 0, VS(11)};

    int max_points = 0;
    search(team, restrictions, all_players, output_file, max_points, -1, 0);
}