#include <iostream>
#include <vector>
#include <algorithm>
#include <functional>
#include <utility>
#include <dirent.h>
#include <fstream>
#include <string>
#include <time.h>

using namespace std;

//classe que representa uma instancia

struct route {
    vector<int>* vertices;
    int time;
    int charge;
};

class archive {
    
    public:

        /**
         * aloca as informacoes de uma instancia nesse objeto
         * @param file_name nome do arquivo da instancia
        */
        archive(string file_name) {
            string aux;
            ifstream file("instances/"+file_name);
            file>>aux>>name;
            file>>aux>>location;
            file>>aux;
            getline(file, comment);
            comment = comment.substr(2);
            file>>aux>>type;
            file>>aux>>size;
            file>>aux;
            getline(file, distribution);
            distribution = distribution.substr(1);
            file>>aux>>depot;
            file>>aux>>route_time;
            file>>aux>>time_window;
            file>>aux>>capacity;
            
            file>>aux;
            vertices = new float*[size];
            for (int i=0; i < size; i++) {

                file>>aux;
                vertices[i] = new float[8];
                for(int j=0; j < 8; j++) {
                    file>>vertices[i][j];
                }
            }

            file>>aux;
            travel_time = new int*[size];
            for (int i=0; i < size; i++) {
                
                travel_time[i] = new int[size];
                for(int j=0; j < size; j++) {
                    file>>travel_time[i][j];
                }
            }
            
        }

        //destrutor
        ~archive() {
            for(int i=0; i < size; i++) {
                delete[] vertices[i];
                delete[] travel_time[i];
            }
            delete[] vertices;
            delete[] travel_time;
        }

        //retorna um vertice pelo seu id
        float* get_vertice(int id) {
            return vertices[id];
        }

        //retorna o tempo de viagem entre dois vertices
        int** get_time() {
            return travel_time;
        }

        int get_capacity() {
            return capacity;
        }

        int get_size() {
            return size;
        }

        pair<vector<route>*, vector<route>*> solutions();

    private:
        //identificacaoo da instancia
        string name;
        //cidade onde foi gerado
        string location;
        //referencia geral
        string comment;
        //tipo da instancia
        string type;
        //numero de vertices
        int size;
        //distribuicao a qual a instancia foi gerada 
        string distribution;
        //tipo de localizacao do depensito: central o aleatorio
        string depot;
        //horizonte de tempo da roteirizacao
        int route_time;
        //largura da janela de tempo
        int time_window;
        //capacidade máxima do veículo
        int capacity;
        /**
         * Vetor que armazena as informacoes dos vertices em vores de tamanho 8, sendo cada posicao:
         * 0: latitude deste local;
         * 1: longitude deste local;
         * 2: a demanda deste no (dem > 0 para coleta, dem < 0 para entrega);
         * 3: tempo mais cedo possível para iniciar o serviço (janela de tempo);
         * 4: ultima hora possivel para iniciar o servico (janela de tempo);
         * 5: a duracao do servico neste local;
         * 6: o par de coleta se <id> for uma entrega; e 0 caso contrario;
         * 7: o par de entrega se <id> for uma coleta; e 0 caso contrario
        */
        float** vertices;
        //matriz que armazena o tempo de viagem entre cada par de vertices da instancia
        int** travel_time;

        //retorna o tempo gasto em um vertice, caso o tempo seja maior que o limite retorna erro de logica
        int bigest_time(int time, int vert) {
            if(time > vertices[vert][4]) {
                logic_error over_time("O caminhao nao chega a tempo");
                throw over_time;
            }
            else if(time < vertices[vert][3]) {
                return vertices[vert][3]+vertices[vert][5];
            }
            else {
                return time+vertices[vert][5];
            }
        }
        bool bigest_charge(int charge) {
            if(charge < capacity)
                return true;
            else
                return false;
        }
        int find_nearest_neighbor(route* new_route, bool* visited, int vertex, int* time, int* charge);
        route nearest_neighbor(route* old_route);
        vector<route>* first_solution(vector<route>* routes);
        vector<route>* second_solution(vector<route>* routes);
        void output(pair<vector<route>*, vector<route>*>* solutions);
};

// Encontra o índice do ponto mais próximo em relação a um ponto dado
int archive::find_nearest_neighbor(route* old_route, bool* visited, int vertex, int* time, int* charge) {
    int near = -1;
    int new_time = __INT_MAX__; // = 2147483647

    // Procura o ponto mais próximo não visitado
    for (int i = 1; i < int(old_route->vertices->size())-1; i++) {
        if (!visited[i] and bigest_charge((*charge)+vertices[old_route->vertices->at(i)][2])) {
            try {
                int d = bigest_time((*time)+travel_time[vertex][i], old_route->vertices->at(i));
                if(vertices[old_route->vertices->at(i)][6] != 0) {
                    auto it = find(old_route->vertices->begin(), old_route->vertices->end(), vertices[old_route->vertices->at(i)][6]);
                    if (it != old_route->vertices->end()) {
                        int pair = it - old_route->vertices->begin();
                        if (d < new_time and visited[pair]) {
                            new_time = d;
                            near = i;
                        }
                    }
                }
                else {
                    if (d < new_time) {
                        new_time = d;
                        near = i;
                    }
                }
                
            }
            catch(logic_error &e){
                //nada acontece
            }
        }
    }
    if(near == -1) {
        logic_error not_find("Nenhum valor encontrado");
        throw not_find;
    }
    (*charge)+= vertices[old_route->vertices->at(near)][2];
    (*time) = new_time;
    return near;
}

// Algoritmo do Vizinho Mais Próximo
route archive::nearest_neighbor(route* old_route) {
    bool visited[old_route->vertices->size()];
    for (int i = 0; i < int(old_route->vertices->size()); i++) visited[i] = false;

    route new_route;
    new_route.vertices = new vector<int>;
    new_route.time = 0;
    new_route.charge = 0;
    // Escolhe um ponto inicial aleatório e adiciona à rota
    int vertex = 0;
    visited[vertex] = true;
    new_route.vertices->push_back(0);
    // Itera até que todos os pontos tenham sido visitados
    while (new_route.vertices->size() < old_route->vertices->size()-1) {
        // Encontra o ponto mais próximo não visitado e adiciona à rota
        vertex = find_nearest_neighbor(old_route, visited, vertex, &new_route.time, &new_route.charge);
        new_route.vertices->push_back(old_route->vertices->at(vertex));
        
        visited[vertex] = true;
    }
    new_route.vertices->push_back(0);

    return new_route;
}

vector<route>* archive::first_solution(vector<route>* routes) {
    bool its_possible = true;
    vector<route>* old_routes = new vector<route>(*routes);
    vector<route>* new_routes;

    while(its_possible) {
        new_routes = new vector<route>; 
        int size = old_routes->size();
        bool remove_last = false;
        if(size%2 == 1) {
            --size;
            remove_last = true;
        }
        for (int j = 0; j < size; j+=2) {
            try {
                route* try_route = new route;
                vector<int>* try_vertices = new vector<int>;
                for (int k = 0; k < int((*old_routes)[j].vertices->size())-1; k++) try_vertices->push_back((*old_routes)[j].vertices->at(k));
                for (int k = 1; k < int((*old_routes)[j+1].vertices->size()); k++) try_vertices->push_back((*old_routes)[j+1].vertices->at(k));
                
                try_route->vertices = try_vertices;
                (*try_route) = nearest_neighbor(try_route);
                new_routes->push_back(*try_route);
                
            }
            catch(logic_error& e) {
                route *route1 = new route, *route2 = new route;
                
                route1->vertices = new vector<int>(*(*old_routes)[j].vertices);
                new_routes->push_back(*route1);
                route2->vertices =new vector<int>(*(*old_routes)[j+1].vertices);
                new_routes->push_back(*route2);
            }
        }
        if(remove_last) {
            route* last_route = new route;
            last_route->vertices = new vector<int>(*(*old_routes).back().vertices);
            new_routes->push_back(*last_route);
        }
        if(int(new_routes->size()) == int(old_routes->size())) its_possible = false;
        delete old_routes;
        old_routes = new_routes;    
    }
    return old_routes;
}

vector<route>* archive::second_solution(vector<route>* routes) {
    bool its_possible = true;
    vector<route>* old_routes = new vector<route>(*routes);
    vector<route>* new_routes;

    while(its_possible) {
        new_routes = new vector<route>; 
        int size = old_routes->size();
        bool remove_last = false;
        if(size%2 == 1) {
            --size;
            remove_last = true;
        }
        for (int j = 0; j < size/2; j++) {
            try {
                route* try_route = new route;
                vector<int>* try_vertices = new vector<int>;
                for (int k = 0; k < int((*old_routes)[j].vertices->size())-1; k++) try_vertices->push_back((*old_routes)[j].vertices->at(k));
                for (int k = 1; k < int((*old_routes)[size-j-1].vertices->size()); k++) try_vertices->push_back((*old_routes)[size-j-1].vertices->at(k));
                
                try_route->vertices = try_vertices;
                (*try_route) = nearest_neighbor(try_route);
                new_routes->push_back(*try_route);
                
            }
            catch(logic_error& e) {
                route *route1 = new route, *route2 = new route;
                
                route1->vertices = new vector<int>(*(*old_routes)[j].vertices);
                new_routes->push_back(*route1);
                route2->vertices =new vector<int>(*(*old_routes)[size-j-1].vertices);
                new_routes->push_back(*route2);
            }
        }
        if(remove_last) {
            route* last_route = new route;
            last_route->vertices = new vector<int>(*(*old_routes).back().vertices);
            new_routes->push_back(*last_route);
        }
        if(int(new_routes->size()) == int(old_routes->size())) its_possible = false;
        delete old_routes;
        old_routes = new_routes;    
    }
    return old_routes;
}

//função para instanciar as rotas dos caminhões 
pair<vector<route>*, vector<route>*> archive::solutions() {
    vector<route>* routes = new vector<route>;
    //marca todos os vertices como false menos o 0
    bool par[size];
    par[0] = true;
    for(int i = 1; i < size; i++) par[i] = false;
    //percorre todos os vertices atribuindo os mesmos e seus pares a uma rota e os marcando com true
    for(int i = 1; i < size; i++) {
        //utiliza try catch para verificar que uma rota pode ser criada sem erros
        try {
            route new_route;
            vector<int>* nodes = new vector<int>;
            nodes->push_back(0);

            //se o vertice ainda nao esta em uma rota e criada uma rota para ele e seu par
            if((vertices[i][6] != 0) and (!par[i] and !par[int(vertices[i][6])])) {
                nodes->push_back(vertices[i][6]);
                nodes->push_back(i);
                
                new_route.time = bigest_time(travel_time[0][int(vertices[i][6])], vertices[i][6]);
                new_route.time = bigest_time(new_route.time+travel_time[int(vertices[i][6])][i], i);
                new_route.time = bigest_time(new_route.time+travel_time[i][0], 0);

                nodes->push_back(0);
                new_route.vertices = nodes;
                routes->push_back(new_route);

                par[i] = true;
                par[int(vertices[i][6])] = true;
                
            }
            else if(!par[i] or !par[int(vertices[i][7])]) {
                nodes->push_back(i);
                nodes->push_back(vertices[i][7]);

                new_route.time = bigest_time(travel_time[0][i], i); 
                new_route.time = bigest_time(new_route.time+travel_time[i][int(vertices[i][7])], vertices[i][7]);
                new_route.time = bigest_time(new_route.time+travel_time[int(vertices[i][7])][0], 0);

                nodes->push_back(0);
                new_route.vertices = nodes;
                routes->push_back(new_route);

                par[i] = true;
                par[int(vertices[i][7])] = true;
            }
            
        }
        catch(logic_error& e) {
            //nao cria nenhuma rota
        }
    }

    sort(routes->begin(), routes->end(), [](route beg, route end) {
        if(beg.time > end.time) return true;
        else return false;
    });

    vector<route>* solution1 = first_solution(routes);
    vector<route>* solution2 = second_solution(routes);

    pair<vector<route>*, vector<route>*> result;
    result.first = solution1;
    result.second = solution2;
    output(&result);
    return result;
}

void archive::output(pair<vector<route>*, vector<route>*> *solutions) {
    vector<route>* solution1 = solutions->first;
    vector<route>* solution2 = solutions->second;

    int trucks1 = solution1->size(), trucks2 = solution2->size(), time1 = 0, time2 = 0;
    for (int i = 0; i < trucks1; i++) time1 += (*solution1)[i].time;
    for (int i = 0; i < trucks2; i++) time2 += (*solution2)[i].time;

    float deviation_m1, deviation_m2, deviation_c1, deviation_c2;
    deviation_m1 = ((trucks1 - trucks2) / trucks2)*100;
    deviation_m2 = ((trucks2 - trucks1) / trucks1)*100;
    deviation_c1 = ((time1 - time2) / time2)*100;
    deviation_c2 = ((time2 - time1) / time1)*100;

    if(deviation_m1 < deviation_m2 or (deviation_m1 == deviation_m2 and deviation_c1 < deviation_c2)) {
        solution2 = solutions->first;
        solution1 = solutions->second;
    }

    cout<<"Primeira solucao"<<endl;
    cout<<"Caminhoes utilizados: "<<solution1->size()<<endl;
    //cout<<"Custo: "<<time1<<endl;
    for (int i = 0; i < int(solution1->size()); i++) {
        for (int j = 0; j < int((*solution1)[i].vertices->size()); j++) {
            cout<<(*solution1)[i].vertices->at(j)<<' ';
        }
        cout<<endl;
    }
    cout<<"--------------------------------------------------------"<<endl;
    cout<<"Melhor solucao"<<endl;
    cout<<"Caminhoes utilizados: "<<solution2->size()<<endl;
    //cout<<"Custo: "<<time2<<endl;
    for (int i = 0; i < int(solution2->size()); i++) {
        for (int j = 0; j < int((*solution2)[i].vertices->size()); j++) {
            cout<<(*solution2)[i].vertices->at(j)<<' ';
        }
        cout<<endl;
    }
    

}

/**
 * verifica se uma solucao esta correta
 * @param instance instancia para qual a solucao foi criada
 * @param solution solucao da isntancia 
 * @param trucks quantidade de caminhoes que foram utilizados na solucao
*/

bool check_solution(archive* instance, vector<route>* solution) {
    int time, weight, wanted, traveled = 0, trucks = solution->size();
    int** travel_time = instance->get_time();
    bool* delivered;
    float* actual;
    
            
    //verifica se a solucao alcanca todos os vertices da instancia
    for (int i = 0; i < trucks; i++) {
        traveled += (*solution)[i].vertices->size() - 2;
    }
    traveled++;
    if(traveled < instance->get_size()) {
        cout<<"A solucao nao percorre todos os pontos da instancia!"<<endl;
        return false;
    }
    else if(traveled > instance->get_size()) {
        cout<<"A solucao percorre algum vertice mais de uma vez!"<<endl;
        return false;
    }
    
    //verifica se a rota de cada caminhao segue os requisitos
    for (int i = 0; i < trucks; i++) {

        //verifica se a rota comeca e termina no deposito
        if((*solution)[i].vertices->front() != 0) {
            cout<<"Ponto de origem do caminhao "<<i+1<<" nao e o deposito!"<<endl;
            return false;
        }
        if((*solution)[i].vertices->back() != 0) {
            cout<<"Ponto final do caminhao "<<i+1<<" nao e o deposito!"<<endl;
            return false;
        }

        //verifica se o caminhao nao passa por um vertice mais de uma vez
        for (int j = 1; j < int((*solution)[i].vertices->size())-1; j++) {
            wanted = (*solution)[i].vertices->at(j);
            for (int k = j+1; k < int((*solution)[i].vertices->size())-1; k++) {
                if((*solution)[i].vertices->at(k) == wanted) {
                    cout<<"O caminhao "<<i<<" passa pelo vertice "<<wanted<<" mais de uma vez!"<<endl;
                    return false;
                }
            }
        }

        time = 0, weight = 0;
        delivered = new bool[(*solution)[i].vertices->size()];
        for (int j = 0; j < int((*solution)[i].vertices->size()); j++) delivered[j] = false;

        //verifica se algum requisito e quebrado em alguma etapa da rota
        for (int j = 1; j < int((*solution)[i].vertices->size()); j++) {
            actual = instance->get_vertice((*solution)[i].vertices->at(j));
            
            //verifica se o tempo limite nao foi ultrapassado
            time += travel_time[(*solution)[i].vertices->at(j-1)][(*solution)[i].vertices->at(j)];
            if(time > actual[4]) {
                cout<<"O caminhao "<<i<<" atrasou para chegar no vertice "<<(*solution)[i].vertices->at(j)<<'!'<<endl;
                delete[] delivered;
                return false;
            }
            else if(time < actual[3]) {
                time = actual[3];
            }

            //verifica se a capacidade nao e ultrapassada 
            weight += actual[2];
            if(weight > instance->get_capacity()) {
                cout<<"O limite de carga do caminhao "<<i<<" foi ultrapassada no vertice "<<(*solution)[i].vertices->at(j)<<'!'<<endl;
                delete[] delivered;
                return false;
            }

            //verifica se a relacao entre coleta e entrega esta correta
            if(actual[6] != 0 and !delivered[j]) {
                cout<<"O caminhao "<<i<<" chegou no vertice "<<(*solution)[i].vertices->at(j)<<" antes de do item do mesmo ser coletado!"<<endl;
                delete[] delivered;
                return false;
            }
            else if(actual[7] != 0) {
                wanted = actual[7];
                for (int k = j+1; k < int((*solution)[i].vertices->size()); k++) {
                    if((*solution)[i].vertices->at(k) == wanted) {
                        delivered[k] = true;
                        k = (*solution)[i].vertices->size();
                        wanted = 0;
                    }
                }
                if(wanted != 0) {
                    cout<<"O caminhao "<<i<<" nunca entrega o pedido coletado no vertice "<<(*solution)[i].vertices->at(j)<<'!'<<endl;
                    delete[] delivered;
                    return false;
                }
            }
        
        }
        delete[] delivered;
    }
    return true;
}

vector<int>* solution_rand(archive* instance, int trucks) {
    int aux;
    vector<int>* solution = new vector<int>[trucks];
    for (int i = 0; i < trucks; i++) {
        solution[i].push_back(0);
    }
    for (int i = 1; i < instance->get_size(); i++) {
        aux = rand() % (trucks);
        solution[aux].push_back(rand()%instance->get_size());
    }
    for (int i = 0; i < trucks; i++) {
        solution[i].push_back(0);
    }

    for (int i = 0; i < trucks; i++) {
        cout<<"Caminhao "<<i<<": ";
        for (int j = 0; j < int(solution[i].size()); j++) {
            cout<<solution[i][j]<<' ';
        }
        cout<<endl;
    }
        
    return solution;
}

int main() {
    
    // string name;
    // cout<<"Digite o nome de uma instancia: ";
    // cin>>name;
    // arquivo* instance = new archive(name);
    // cout<<"Arquivo acessado com sucesso!"<<endl;

    // int continua, trucks;
    // pair<vector<route>*, vector<route>*> solutions = instance->solutions(); 
    // vector<int>* solution;
    // srand(time(NULL));
    // do {
    //     cout<<"Digite 1 para testar uma solucao aleatoria"<<endl;
    //     cout<<"Digite 2 para testar a instancia"<<endl;
    //     cout<<"Digite 3 para sair"<<endl;
    //     cin>>continua;
    //     if(continua == 1) {
    //         trucks = (rand() % (instance->get_size()/10)) +1, 
    //         solution = solution_rand(instance, trucks);
    //
    //         if(check_solution(instance, solution, trucks)) {
    //             cout<<"Solucao valida!"<<endl;
    //         }
    //     else if(continua == 2) {
    //          if(check_solution(instance, solutions.first)) {
    //              cout<<" "<<endl;
    //          }
    //          if(check_solution(instance, solutions.second)) {
    //              cout<<" "<<endl;
    //          }
    //     }
    //
    //         delete[] solution;
    //         delete instance;
    //     }
    // } while(continua != 3);
    
    string name;
    cout<<"Digite o nome de uma instancia: ";
    cin>>name;
    archive* instance = new archive(name);
    cout<<"Arquivo acessado com sucesso!"<<endl;

    pair<vector<route>*, vector<route>*> solutions = instance->solutions();
    
    if(check_solution(instance, solutions.first)) {
        //cout<<"funfou 1"<<endl;
    }
    if(check_solution(instance, solutions.second)) {
        //cout<<"AEEEEEE POOOORRRA"<<endl;
    }

    delete instance;
    
    return 0;
}