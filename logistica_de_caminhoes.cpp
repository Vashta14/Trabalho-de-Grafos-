#include <iostream>
#include <vector>
#include <algorithm>
#include <functional>
#include <utility>
#include <dirent.h>
#include <fstream>
#include <string>

using namespace std:

class arquivo {
    
    public:

        arquivo(string file_name) {
            string aux;
            ifstream file(file_name);
            file>>aux>>aux;
            this->nome = aux;
            file>>aux>>aux;
            this->location = aux;
            file>>aux;
            getline(file, this->comment)
            file>>aux>>aux;
            this->type = aux;
            file>>aux>>aux;
            this->size = aux;
            file>>aux;
            getline(file, this->distribution);
            file>>aux>>aux;
            this->depot = aux;
            file>>aux>>aux;
            this->route_time = aux;
            file>>aux>>aux;
            this->time_window = aux;
            file>>aux>>aux;
            this->capacity = aux;
            file>>aux;

            int i = 0;
            this->vertices = new int*[this->size];
            while (scanf("%d", aux) != "EDGES") {
                this->vertices[i] = new int[9];
                vertices[i][0] = aux; 

                for(int j=1; j < 9; j++){
                    file>>aux;
                    vertices[i][j] = aux; 
                }
                i++;
            }

            i = 0;
            this->travel_time = new int*[this->size];
            while (scanf("%d", aux) != "EOF") {
                this->travel_time = new int[this->size];
                travel_time[i][0] = aux; 
                
                for(int j=1; j < this->size; j++){
                    file>>aux;
                    travel_time[i][j] = aux; 
                }
                i++;
            }
        }

        ~arquivo() {
            delete nome;
            delete location;
            delete comment;
            delete type;
            delete distribution;
            delete depot;
            delete route_time;
            delete time_window;
            delete capacity;
            for(int i=0; i < size; i++) {
                delete[] vertices[i];
                delete[] travel_time[i];
            }
            delete[] vertices;
            delete[] travel_time;
            delete size;
        }
    
    private:
        string nome;
        string location;
        string comment;
        string type;
        int size;
        string distribution;
        string depot;
        int route_time;
        int time_window;
        int capacity;
        int** vertices;
        int** travel_time;

}

void distribuição() {

}

int main() {
    string nome;
    cout>>"Digite o nome do arquivo:: ";
    cin>>nome;
    arquivo instancia = new arquivo(nome);

}