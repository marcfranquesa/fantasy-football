// Exhaustive search for best team
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
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
*/
struct Team {
    int T, P;
    PlayersByPosition players;
};


// Returns processor time consumed by the program in seconds
double time(){
    return (double) clock() / CLOCKS_PER_SEC;
}


bool player_sorter(const Player& a, const Player& b) {
    if (a.points == b.points) return a.price < b.price;
    return a.points > b.points;
}


/*
    Reads data file containing players, saves all players into "all_players" as long as their price
    is less than or equal to the limit per player given
*/
void read_players(const string& data_file, PlayersByPosition& all_players, const int& limit){
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
        // Ignores players more expensive than limit
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

    for (Players& position : all_players) sort(position.begin(), position.end(), player_sorter);
}


void read_input(int argc, char** argv, Restrictions& restrictions, PlayersByPosition& all_players){
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


// Can also be used to remove from team
void add_player(
    Team& team, const Player& player, const int& position, const int& index, const bool& remove
){
    int multiplier = (remove ? -1 : 1);
    team.P += player.points * multiplier;
    team.T += player.price * multiplier;
    team.players[position][index] = player;
}


bool is_team_worth_pursuing(
    const Team& team, const Restrictions& restrictions, const PlayersByPosition& all_players,
    const Team& best_team, const int& position, const int& player_index,
    const int& players_in_position
){
    int theoretical_max = team.P;
    for (int i = players_in_position + 1; i < restrictions.limits[position]; ++i){
        theoretical_max += all_players[position][player_index + i - players_in_position].points;
    }
    for (int i = position + 1; i < 4; ++i) for (int j = 0; j < restrictions.limits[i]; ++j){
        theoretical_max += all_players[i][j].points;
    }

    return (
        (theoretical_max > best_team.P) and
        (team.T <= restrictions.T)
    );
}


void write_team(
    const Team& team, const Restrictions& restrictions,
    const string& outputFile, const double& start
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


/*
    Exhaustive search algorithm, searching is done incrementally by position,
    first a goalkeeper is placed, then defenders, etc.
    At any moment only players that have assigned the current position
    are searched.
*/
void search(
    Team& team, const Restrictions& restrictions, const PlayersByPosition& all_players,
    const string& output_file, const double& start, Team& best_team,
    const int& position, const int& previous_player_index, const int& players_in_position
){

    if (players_in_position == restrictions.limits[position]){
        if (position == 3) { if (team.P > best_team.P) {
            write_team(team, restrictions, output_file, start);
            best_team = team;
        }}
        else search(team, restrictions, all_players, output_file, start, best_team, position + 1, -1, 0);
        return;
    }

    int options = all_players[position].size();
    for (
        int i = previous_player_index + 1; 
        (i < (int) options) and (options - i >= restrictions.limits[position] - players_in_position);
        ++i
    ){
        add_player(team, all_players[position][i], position, players_in_position, false);
        if (is_team_worth_pursuing(team, restrictions, all_players, best_team, position, i, players_in_position)){
            search(team, restrictions, all_players, output_file, start, best_team, position, i, players_in_position + 1);
        }
        add_player(team, all_players[position][i], position, players_in_position, true);
    }
}


int main(int argc, char** argv) {

    double start = time();
    Restrictions restrictions;
    PlayersByPosition all_players(4);
    read_input(argc, argv, restrictions, all_players);
    const string output_file = argv[3];

    // Initialising empty candidate team
    PlayersByPosition players = {
        Players(restrictions.limits[0]),
        Players(restrictions.limits[1]),
        Players(restrictions.limits[2]),
        Players(restrictions.limits[3])
    };
    Team team = {0, 0, players};
    Team best_team = {0, 0, players};

    search(team, restrictions, all_players, output_file, start, best_team, 0, -1, 0);
}