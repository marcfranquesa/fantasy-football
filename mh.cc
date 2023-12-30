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

    bool isOnTeam(const string& playerName) const {
        for (const VP& playerVector : players) {
            for (const Player& player : playerVector) {
                if (player.name == playerName) {
                    return true;
                }
            }
        }
        return false;
    }
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
Player select_random_player(VP& candidate_list){

    int rnd = rand() % candidate_list.size();

    return candidate_list[rnd];
}

// generates a random_team
Team greedy_randomized_team(const VP& all_players, const Restrictions& restrictions, const int& alpha){

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

    while (size_pos[1] != restrictions.limits[1] or size_pos[2]!= restrictions.limits[2] or size_pos[3] != restrictions.limits[3] or size_pos[0] != 1){
        // creem candidate list de mida alpha de jugadors que hi càpiguen al equip
        int k = 0;
        int j = 0;
        VP CL;
        while (k < alpha and j < all_players.size()){
            // just to make sure that we have a CL even though we don't have >= alpha avaliable players we add the last constriction
            Player p = all_players[j];
            if(p.price + team.T <= restrictions.T and p.price <= restrictions.J and size_pos[p.position] < restrictions.limits[p.position] and
            !team.isOnTeam(p.name)){
                CL.push_back(p);
                k++;
            }  
            j++;
        }

        Player p = select_random_player(CL);
        add_player(team, p, size_pos[p.position]);
    }

    return team;
}


Team find_random_neighbour(Team team, const VP& all_players, const Restrictions& restrictions){

    int found = 0;
    while (found == 0){
        int index_rnd = rand() % all_players.size();
        Player p = all_players[index_rnd];
        if(!team.isOnTeam(p.name)){
            //exchange it with a player in team with same position
            
            int index_deleted = rand() % restrictions.limits[p.position];
            Player p_deleted = team.players[p.position][index_deleted];
            if(team.T + p.price - p_deleted.price < restrictions.T){
                team.T -= p_deleted.price;
                team.P -= p_deleted.points;

                add_player(team, p, index_deleted);
                
                found = 1;
            }
        }
    }

    return team;
}

// find the best team around team
// searching players that fit the team that increases the team's points
Team local_search(const Team& team, const VP& all_players, const Restrictions& restrictions){
    
    Team local_best = team;
    Team actual = team;
    int k = 0;
    // trobo una alineació veí, si és millor l'escullo, sino, amb probabilitat 1-p l'escullo
    while (k < 5000){
        
        Team t = find_random_neighbour(actual, all_players, restrictions);

        if (t.P > actual.P){
            actual = t;

            if (t.P > local_best.P){
                local_best = t;
                k = 0;
            }
        }
        else{
            // escullo un canvi que empitjora amb probabilitat inv. proporcional a l'empitjorament que aquest té
            // Com mes empitjora el random neighour, menys possibilitats d'escollir-lo
            // Generar un número aleatorio real entre 0 y 100
            int rnd = rand() % 100;
            double coef = static_cast<double>(t.P) / actual.P;
            if (rnd < 20 * coef){
                actual = t;
            }
            k++;
        }
    }
    
    return local_best;

}


void metaheuristic(const Restrictions& restrictions, VP& all_players,
            const string& output_file, const double& start){
    Team team, rand_team, best_team;
    int alpha = 4;
    best_team.P = 0;
    int k = 0;

    while(k < 100){
        rand_team = greedy_randomized_team(all_players, restrictions, alpha);
        team = local_search(rand_team, all_players, restrictions);

        if (team.P >= best_team.P) best_team = team;

        k++;
    }
    write_team(best_team, restrictions, output_file, start);
}


int main(int argc, char** argv) {

    double start = time();
    Restrictions restrictions;
    VP all_players;
    read_input(argc, argv, restrictions, all_players);
    const string output_file = argv[3];

    metaheuristic(restrictions, all_players, output_file, start);
}