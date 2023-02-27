
# Trabalho de Grafos

Trabalho realizado para a matéria de Algoritmos em Grafos, onde  problema consiste em encontrar uma rota heurística para atender um conjunto de clientes com demandas diferentes, considerando a capacidade dos veículos, as janelas de tempo, a precedência de coleta e entrega para a minimização do custo total. Esse problema é comum em empresas que precisam otimizar suas rotas de entrega para reduzir os custos logísticos, que tiveram a sua necessidade aumentada durante a pandemia, em que houve um aumento significativo de compras online.


## UNIVERSIDADE FEDERAL DE LAVRAS
INSTITUTO DE CIÊNCIAS EXATAS E TECNOLÓGICAS  
DEPARTAMENTO DE CIÊNCIAS DA COMPUTAÇÃO  
GCC218 — ALGORITMOS EM GRAFOS  
Prof. Mayron César de O. Moreira

## Autores

- Gilson dos Santos Júnior - 14A - [@RATZogun](https://github.com/RATZogun)
- Gustavo	Henrique Marques Magalhães - 14A - [@Gustavohmmagalhaes](https://github.com/Gustavohmmagalhaes)
- Raul Souza Lima - 14A - [@Vashta14](https://github.com/Vashta14)


## Estrutura

Para a resolução do problema foi definido um sistema de classe utilizado para a leitura de dados direto do arquivo texto. Os dados então são armazenados em vetores de tamanho 8, sendo cada posição:  
0: latitude;  
1: longitude;  
2: demanda do local ("dem" > 0 para coleta, "dem" < 0 para entrega);  
3: tempo mais cedo possível para iniciar o serviço (janela de tempo);  
4: ultima hora possível para iniciar o serviço (janela de tempo);  
5: a duração do serviço neste local;  
6: o par de coleta se id for uma entrega e 0 caso contrário;  
7: o par de entrega se id for uma coleta e 0 caso contrário.  

- find_nearest_neighbor: recebe como parâmetro um ponto e procura o índice do ponto mais próximo não visitado.  
- nearest_neighbor: executa o algoritmo do vizinho mais próximo; escolhe um ponto inicial aleatório e adiciona à rota; chama a função find_nearest_neighbor passando esse ponto aleatório.
- first_solution: 
- second_solution:
- solutions: atribui as rotas dos caminhões; chama a função output passando a rota atribuida.
- output: calcula o desvio para definir a melhor solução gerada; organiza um saída pra o terminal com as duas soluções obtidas e o número de caminhões gasto em cada uma delas.
- check_solution: utiliza de várias verificações para garantir que a solução encontrada é uma solução válida, respeitando as condições pré-impostas de precedência de coleta e entrega, origem e horário de serviço, janelas de tempo, obrigatoriedade e exclusividade de visita, atendimento de pedido e capacidade de veículo.
- solution_rand: gera uma solução aleatória para fins de depuração do código.

## Exemplo

A imagem a seguir exemplifica a ideia geral de funcionamento seguida no desenvolvimento desse código.
![App Screenshot](https://i.imgur.com/eNwsZu6.png)
