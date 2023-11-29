#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <cassert>
#include <vector>
using namespace std;


struct Player {
    string name;
    string position;
    int price;
    string club;
    int points;
};
using VP = vector<Player>;


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
    // N0: current goalkeepers
    // N1: current defenders
    // N2: current midfielders
    // N3: current attackers
    // T: current cost
    // P: current points
    // players: vector with players used
    int N0, N1, N2, N3, T, P;
    VP players;
};


// Reads data file containing players, saves all players into "players" as long as their price
// is less than or equal to the limit per player given
void read_players(const string& data_file, VP& all_players, int limit){
    ifstream input(data_file);
    while (not input.eof()) {
        Player player;
        auto& [name, position, price, club, points] = player;
        getline(input, name, ';');    if (name == "") break;
        getline(input, position, ';');
        input >> price;
        char aux; input >> aux;
        getline(input, club, ';');
        input >> points;
        string aux2;
        getline(input, aux2);
        if (price <= limit) all_players.push_back(player);
    }
    input.close();
}


void read_input(int argc, char** argv, Restrictions& restrictions, VP& all_players){
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


void add_player(Team& team, const Player& player, int index, bool remove){
    
    int multiplier;
    if (remove) {
        multiplier = -1;
        team.players[index] = player;
    }
    else {
        multiplier = 1;
    }

    team.P += player.points * multiplier;
    team.T += player.price * multiplier;
    if (player.position == "por") team.N0 += multiplier;
    else if (player.position == "def") team.N1 += multiplier;
    else if (player.position == "mig") team.N2 += multiplier;
    else if (player.position == "dav") team.N3 += multiplier;
    else throw invalid_argument("invalid position");
}


bool is_team_valid(const Team& team, const Restrictions& restrictions, int current_players, int max_points){
    return (
        (team.N0 <= restrictions.N0) and
        (team.N1 <= restrictions.N1) and
        (team.N2 <= restrictions.N2) and
        (team.N3 <= restrictions.N3) and
        (team.T <= restrictions.T) and 
        ((team.P + (11 - current_players) * restrictions.J) > max_points)
    );
}


void write_team(const Team& team, const string& outputFile){
    /*
    finishTime = std::clock();
    duration = ( std::clock() - start ) / (double) CLOCKS_PER_SEC;
    */
    Player POR;
    vector<Player> DEF;
    vector<Player> MIG;
    vector<Player> DAV;
    for(Player J : team.players){
        if (J.position == "por") POR = J;
        else if(J.position == "def") DEF.push_back(J);
        else if(J.position == "mig") MIG.push_back(J);
        else if(J.position == "dav") DAV.push_back(J);
    }

    ofstream File;
    File.open(outputFile);


    File << "POR: " << POR.name << endl;
    File << "DEF: ";
    for(Player J : DEF) File << J.name << ";";
    File << endl << "MIG:";
    for(Player J : MIG) File << J.name << ";";
    File << endl << "DAV:";
    for(Player J : DAV) File << J.name << ";";
    File << endl;
    File << "Punts: " << team.P << endl;
    File << "Preu: " << team.T << endl;

    File.close();
}


void search(Team& team, const Restrictions& restrictions, const VP& all_players, const string& output_file, 
            int& max_points, int previous_player_index, int current_players){
    
    cout << current_players << endl;
    if (current_players == 11){
        if (team.P > max_points) {
            write_team(team, output_file);
            max_points = team.P;
        }
        return;
    }
    for (int i = previous_player_index + 1; i < (int) all_players.size(); ++i) {
        add_player(team, all_players[i], current_players, false);
        if (is_team_valid(team, restrictions, current_players + 1, max_points)){
            search(team, restrictions, all_players, output_file, max_points, i, current_players + 1);
        }
        add_player(team, all_players[i], current_players, true);
    }
}


int main(int argc, char** argv) {

    Restrictions restrictions;
    VP all_players;
    read_input(argc, argv, restrictions, all_players);
    const string output_file = argv[3];

    Team team = {0, 0, 0, 0, 0, 0, VP(11)};
    
    int max_points = 0;
    search(team, restrictions, all_players, output_file, max_points, -1, 0);
}