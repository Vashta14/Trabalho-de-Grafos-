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
class arquivo {
    
    public:

        /**
         * aloca as informacoes de uma instancia nesse objeto
         * @param file_name nome do arquivo da instancia
        */
        arquivo(string file_name) {
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
                for(int j=0; j < 8; j++){
                    file>>vertices[i][j];
                }
            }

            file>>aux;
            travel_time = new int*[size];
            for (int i=0; i < size; i++) {
                
                travel_time[i] = new int[size];
                for(int j=0; j < size; j++){
                    file>>travel_time[i][j];
                }
            }
            
        }

        //destrutor
        ~arquivo() {
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

};

/**
 * verifica se uma solucao esta correta
 * @param instance instancia para qual a solucao foi criada
 * @param solution solucao da isntancia 
 * @param trucks quantidade de caminhoes que foram utilizados na solucao
*/
bool verificar_solucao(arquivo* instance, vector<int>* solution, int trucks) {
    int time, weight, wanted, traveled = 0;
    int** travel_time = instance->get_time();
    bool* delivered;
    float* actual;
            
            
    //verifica se a solucao alcanca todos os vertices da instancia
    for (int i = 0; i < trucks; i++) {
        traveled += solution[i].size() - 2;
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
        if(solution[i].front() != 0) {
            cout<<"Ponto de origem do caminhao "<<i+1<<" nao e o deposito!"<<endl;
            return false;
        }
        if(solution[i].back() != 0) {
            cout<<"Ponto final do caminhao "<<i+1<<" nao e o deposito!"<<endl;
            return false;
        }

        //verifica se o caminhao nao passa por um vertice mais de uma vez
        for (int j = 1; j < solution[i].size()-1; j++){
            wanted = solution[i][j];
            for (int k = j+1; k < solution[i].size()-1; k++) {
                if(solution[i][k] == wanted) {
                    cout<<"O caminhao "<<i<<" passa pelo vertice "<<wanted<<" mais de uma vez!"<<endl;
                    return false;
                }
            }
        }

        time = 0, weight = 0;
        delivered = new bool[solution[i].size()];
        for (int j = 0; j < solution[i].size(); j++) delivered[j] = false;

        //verifica se algum requisito e quebrado em alguma etapa da rota
        for (int j = 1; j < solution[i].size(); j++){
            actual = instance->get_vertice(solution[i][j]);
            
            //verifica se o tempo limite nao foi ultrapassado
            time += travel_time[solution[i][j-1]][solution[i][j]];
            if(time > actual[4]) {
                cout<<"O caminhao "<<i<<" atrasou para chegar no vertice "<<solution[i][j]<<'!'<<endl;
                return false;
            }
            else if(time < actual[3]) {
                time = actual[3];
            }

            //verifica se a capacidade nao e ultrapassada 
            weight += actual[2];
            if(weight > instance->get_capacity()) {
                cout<<"O limite de carga do caminhao "<<i<<" foi ultrapassada no vertice "<<solution[i][j]<<'!'<<endl;
                return false;
            }

            //verifica se a relacao entre coleta e entrega esta correta
            if(actual[6] != 0 and !delivered[j]) {
                cout<<"O caminhao "<<i<<" chegou no vertice "<<solution[i][j]<<" antes de do item do mesmo ser coletado!"<<endl;
                return false;
            }
            else if(actual[7] != 0) {
                wanted = actual[7];
                for (int k = j+1; k < solution[i].size(); k++) {
                    if(solution[i][k] == wanted) {
                        delivered[k] = true;
                        k = solution[i].size();
                        wanted = 0;
                    }
                }
                if(wanted != 0) {
                    cout<<"O caminhao "<<i<<" nunca entrega o pedido coletado no vertice "<<solution[i][j]<<'!'<<endl;
                    return false;
                }
            }
        
        }
        delete[] delivered;
    }
    return true;
}

vector<int>* solution_rand(arquivo* instance, int trucks) {
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

    for (int i = 0; i < trucks; i++)
    {
        cout<<"Caminhao "<<i<<": ";
        for (int j = 0; j < solution[i].size(); j++)
        {
            cout<<solution[i][j]<<' ';
        }
        cout<<endl;
    }
        
    return solution;
}

int main() {
    string name;
    cout<<"Digite o nome de uma instancia: ";
    cin>>name;
    arquivo* instance = new arquivo(name);
    cout<<"Arquivo acessado com sucesso!"<<endl;

    int continua, trucks;
    vector<int>* solution;
    srand(time(NULL));
    do {
        cout<<"Digite 1 para testar uma solucao aleatoria"<<endl;
        cout<<"Digite 2 para sair"<<endl;
        cin>>continua;
        if(continua == 1) {
            trucks = (rand() % (instance->get_size()/10)) +1, 
            solution = solution_rand(instance, trucks);

            if(verificar_solucao(instance, solution, trucks)) {
                cout<<"Solucao valida!"<<endl;
            }
        }
    }while(continua == 1);
     

}