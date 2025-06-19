# Atividade02 — Distância de Edição entre Árvores

## Descrição

Implementação o algoritmo de **Zhang-Shasha** para cálculo da **distância de edição entre árvores** (*tree edit distance*).

O algoritmo determina o menor número de operações (inserção, deleção e substituição de nós) necessárias para transformar uma árvore enraizada em outra, usando programação dinâmica. É amplamente utilizado em bioinformática, análise de XML, linguística computacional, entre outros.

---

## Lógica do Algoritmo (Zhang-Shasha)

**1. Numeração em pós-ordem e cálculo do leftmost leaf descendant (LLD):**
- Funções: `computePostorder`, `computeLLD`, `buildTreeInfo`
- Fundamentais para identificar subflorestas e estruturar a programação dinâmica.

**2. Programação dinâmica para subflorestas e árvores:**
- Núcleo do algoritmo: laços aninhados em `treeEditDistance`
- Preenchimento das matrizes `forestdist` e `treedist`
- Recorrência de Zhang-Shasha: deleção, inserção, substituição, e subproblemas de subflorestas.

**3. Recuperação do resultado:**
- O valor final da distância de edição está em `treedist[n-1][m-1]`.

---

## Como compilar e executar

### Pré-requisitos

- Compilador C++ (recomendado: g++ 9 ou superior)
- CMake 3.10 ou superior

### Passos

1. **Clone ou copie este repositório.**

2. **Compile o projeto:**
   ```sh
   mkdir build
   cd build
   cmake ..
   make
   ```

3. **Execute o programa:**
   ```sh
   ./Atividade02
   ```

   O programa irá:
   - Gerar duas árvores aleatórias.
   - Calcular e mostrar a distância de edição entre elas.
   - Exibir a matriz de distâncias e estatísticas da execução.

---

## Personalização

Para alterar o número mínimo de nós das árvores modificando os argumentos em `generateRandomTree()` no `main.cpp`.

Para ver logs detalhados do algoritmo, basta passar `true` como terceiro argumento para `treeEditDistance`.

---

## Referência

- Zhang, K., & Shasha, D. (1989). Simple fast algorithms for the editing distance between trees and related problems. SIAM Journal on Computing, 18(6), 1245-1262.

---