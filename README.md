# Componentes Conexas em Paralelo

## Introdução

Seja G = (V, E) um grafo representado por uma lista de adjacências, o objetivo deste relatório é descobrir em paralelo se G é conexo e se não for quantas e quais são suas componentes conexas. 

Um grafo é conexo se para cada par (u, v) de seus vértices, existe um caminho com origem u e término v. Tome como exemplo o grafo G1 a seguir, onde G1 é numerado da esquerda pra direita, e de cima para baixo, com número contínuos de 1 até |V| para todos os seus vértices.

![grafo g1](/images/image1.png)

Veja que a partir do vértice 1 não existe nenhum caminho para o vértice 4. Isso indica que G1 não é conexo. Contudo, a partir do vértice 1 é possível alcançar os vértices 2, 3, 7, 8, 9, e 10, implicando que esses vértices formam um pedaço (ou componente) conexo do meu grafo G1. Visualmente podemos verificar que existem 3 componentes conexas no grafo G1, mas como determinar isso através de um algoritmo?

## Abordagem Sequencial

Sequencialmente podemos resolver esse problema usando k buscas em profundidade, onde k é o número de componentes conexas do meu grafo. Dada uma raiz r, a busca em profundidade visita todos os vértices conectados a essa raiz (ou seja, acha uma componente conexa). Se após realizarmos uma busca em profundidade, procurarmos por vértices não visitados e repetirmos a busca caso algum exista, podemos encontrar todas as componentes conexas de um grafo. Isso gera o seguinte algoritmo:

```
k ← 0
Para todo v em V, marque v como não visitado
	Para todo v em V: 
		Se v ainda não foi visitado
			k ← k + 1 
			dfs(v)
```

O algoritmo acima possui uma complexidade de tempo de O(m + n), onde m é o número de arestas e n é o número de vértices e será usado para comparar a eficiência do algoritmo paralelo implementado para este relatório.

## Abordagem Paralela

Existe um algoritmo chamado *Randomized Parallel Connected Components*[1] que serve para achar as componentes conexas de um grafo. Esse algoritmo foi escolhido para ser implementado neste relatório. 

Seja G1 o grafo dado a seguir, podemos achar as componentes conexas de G1 da seguinte forma: 

![grafo g1](/images/image1.png)

**Passo 1:** Para cada vértice v em V, jogue uma moeda aleatoriamente. Se der cara, marque esse vértice como um filho. Se der coroa, marque-o como pai.

![grafo g1](/images/image4.png)
Os vértices marcados com um X vermelho são pais.

**Passo 2:** Para cada aresta (u, v) em E, se u é um pai e v é um filho, faça o filho apontar para o pai. 

![grafo g1](/images/image3.png)

O conjunto formado por cada um dos pais e dos seus filhos formam um possível candidato a componente conexa.

![grafo g1](/images/image8.png)

**Passo 3:** Junte cada pai com seus filhos em um único vértice. Isso é possível de ser feito com uma soma de prefixos. 

![grafo g1](/images/image2.png)

**Passo 4:** Nesse novo grafo formado, se o número de arestas for 0 vá para o passo 5, senão repita o passo 1.

#### Iteração 2:

![grafo g1](/images/image5.png)
![grafo g1](/images/image11.png)
![grafo g1](/images/image10.png)
![grafo g1](/images/image9.png)


#### Iteração 3:

![grafo g1](/images/image8.png)
![grafo g1](/images/image6.png)
![grafo g1](/images/image12.png)

**Passo 5:** Quando o número de arestas for 0, a quantidade de componentes conexas foi encontrada e é possível mapear de volta para o grafo original. 

![grafo g1](/images/image13.png)

### Pseudo-código

Dado um grafo G = (V, E), onde |V| = n, |E| = m. Seja L meu vetor resposta, C o vetor que guarda se um vértice é pai ou filho e S o vetor da soma de prefixos, o passo a passo explicado acima pode ser traduzido no seguinte pseudo-código:

```
randomizedCC(L, V, E)
	Inicialize os arrays C[1..n], L[1..n] = V, S[1..m]
	Se m = 0, então retorne L
	Para v de 1 até n em paralelo
		C[v] = random{ PAI, FILHO }
	Para (u, v) em E em paralelo
		Se C[u] é um pai e C[v] é um filho
			L[u] = L[v] 
	Para i de um até m em paralelo
		Se L[E[i].u] != L[E[i].v]
			S[i] = 1
		Senão
			S[i] = 0
	S = soma_prefixos(S, +)
	Inicialize F[ 1.. |S[n]| ]
	Para i de um até m em paralelo
		Se L[E[i].u] != L[E[i].v]
			F[S[i]] = (L[E[i].u],L[E[i].v])
	L = randomizedCC(L, V, E)
	Para (u, v) em E em paralelo
		Se v = L[u]
			L[u] = L[v]
	Retorne L
```

### Implementação

