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
    int position;
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

    sort(all_players.begin(), all_players.end(), player_sorter);
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


// Can also be used to remove from team
void add_player(
    Team& team, const Player& player, vector<int>& players_per_position, const bool& remove
){
    int multiplier = -1;
    int position = player.position;
    if (not remove) {
        team.players[position][players_per_position[position]] = player;
        multiplier = 1;
    }
    team.P += player.points * multiplier;
    team.T += player.price * multiplier;
    players_per_position[position] += multiplier;
}


/*
    Exhaustive search prunes, if either of the following conditions are met:

    * We have already reached the limit of players for the specific position
    * The player's price would make the teams price exceed the total limit
    * The theoretical maximum points of the team would be less than the current best team
    
    return false, otherwise return true
*/
bool is_team_worth_pursuing(
    const Team& team, const Restrictions& restrictions, const Players& all_players,
    const Team& best_team, const int& player_index, const int& total_players,
    const vector<int>& players_per_position
){
    int position = all_players[player_index].position;

    if (players_per_position[position] == restrictions.limits[position]) return false;
    if (team.T + all_players[player_index].price > restrictions.T) return false;

    int theoretical_max = team.P + all_players[player_index].points;
    theoretical_max += all_players[player_index + 1].points * (10 - total_players);

    if (theoretical_max <= best_team.P) return false;

    return true;
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


// Exhaustive search algorithm, searches for the best team possible
void search(
    Team& team, const Restrictions& restrictions, const Players& all_players,
    const string& output_file, const double& start, Team& best_team,
    const int& previous_player_index, const int& total_players, vector<int>& players_per_position
){
    if (total_players == 11){
        // No need to check any other condition as this is done
        // before calling the function in "is_team_worth_pursuing"
        write_team(team, restrictions, output_file, start);
        best_team = team;
        return;
    }

    int options = all_players.size();
    for (int player_index = previous_player_index + 1; player_index < (int) options; ++player_index){
        if (is_team_worth_pursuing(team, restrictions, all_players, best_team, player_index, total_players, players_per_position)){
            add_player(team, all_players[player_index], players_per_position, false);
            search(team, restrictions, all_players, output_file, start, best_team, player_index, total_players + 1, players_per_position);
            add_player(team, all_players[player_index], players_per_position, true);
        }
    }
}


int main(int argc, char** argv) {

    double start = time();
    Restrictions restrictions;
    Players all_players;
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
    vector<int> players_per_position = {0, 0, 0, 0};

    search(team, restrictions, all_players, output_file, start, best_team, -1, 0, players_per_position);
}