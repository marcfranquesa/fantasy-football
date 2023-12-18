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


// Defined as a class to be able to initialise it with parameters
class comp {
    double c1, c2;
    int remaining_money, total_budget, max_points;
    public:
        comp(double c1, double c2, int remaining_money, int total_budget, int max_points)
            : c1(c1), c2(c2), remaining_money(remaining_money), total_budget(total_budget), max_points(max_points){}

        double value(const Player& p) const {
            return pow(p.points, c1) * pow((remaining_money - p.price), c2);
        }

        bool operator()(const Player& p1, const Player& p2) const {
            if (p1.points == 0 and not (p2.points == 0)) return true;
            if (p2.points == 0 and not (p1.points == 0)) return false;
            return value(p1) < value(p2);
        }
};


// Reads data file containing players, saves all players into "all_players" as long as their price
// is less than or equal to the limit per player given
void read_players(const string& data_file, VP& all_players, const int& limit){
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
    ++index;
}


void write_team(const Team& team, const Restrictions& restrictions,
                const string& outputFile, const double& start){
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


void greedy(const Restrictions& restrictions, VP& all_players,
            const string& output_file, const double& start){

    // Initialising empty team
    VVP players = {
        VP(restrictions.limits[0]),
        VP(restrictions.limits[1]),
        VP(restrictions.limits[2]),
        VP(restrictions.limits[3])
    };

    Team team = {0, 0, players};
    VI size_pos = {0, 0, 0, 0};
    Player p;

    // Parameters used to sort the players
    double c1 = 1, c2 = 1.5;
    
    // While team is not full, we sort the players and add the first
    // player that satisfies the restrictions, at each iteration
    // the parameters are changed
    for (int total_players = 0; total_players < 11; ++total_players) {
        // Sorts players in increasing order, check comp class
        // for exact sorting criteria
        sort(
            all_players.begin(),
            all_players.end(),
            comp(c1, c2, restrictions.T - team.T, restrictions.T, restrictions.J)
        );

        // Finds first possible player, until found, removes
        // players that are not usable
        do {
            p = all_players[all_players.size() - 1];
            all_players.pop_back();
        } while (
            (team.T + p.price > restrictions.T) or
            not (restrictions.limits[p.position] > size_pos[p.position])
        );

        add_player(team, p, size_pos[p.position]);
        c1 *= (total_players + 12)/11;
    }

    write_team(team, restrictions, output_file, start);
}


int main(int argc, char** argv) {

    double start = time();
    Restrictions restrictions;
    VP all_players;
    read_input(argc, argv, restrictions, all_players);
    const string output_file = argv[3];

    greedy(restrictions, all_players, output_file, start);
}