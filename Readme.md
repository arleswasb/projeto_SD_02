Disciplina: Programação Paralela
Professor: Samuel Xavier de Souza
Projeto PP 02

Descrição da Tarefa
Implemente três laços em C para investigar os efeitos do paralelismo ao nível de instrução (ILP): 1) inicialize um vetor com um cálculo simples; 2) some seus elementos de forma acumulativa, criando dependência entre as iterações; e 3) quebre essa dependência utilizando múltiplas variáveis. Compare o tempo de execução das versões compiladas com diferentes níveis de otimização (O0, O2, O3) e analise como o estilo do código e as dependências influenciam o desempenho.

Objetivo do Teste
Este estudo visa demonstrar e quantificar como a estrutura do código-fonte, especialmente a presença de dependências de dados, impacta a capacidade do processador de explorar o Paralelismo em Nível de Instrução (ILP). O experimento correlaciona o tempo de execução de diferentes laços com as otimizações do compilador e a microarquitetura de processadores superescalares modernos.

O Experimento: Dependência de Dados vs. ILP
Para isolar o efeito das dependências, foram implementados quatro laços distintos que operam sobre um vetor de 100 milhões de elementos.

Laço 1: Inicialização do Vetor
for (int i = 0; i < TAMANHO_VETOR; i++) {
    vetor[i] = (i % 10) + 1;
}

Análise Arquitetónica: Este laço é altamente paralelizável. Cada iteração é totalmente independente das outras, permitindo que o processador execute múltiplas operações de escrita em paralelo através de técnicas como pipelining e execução out-of-order.

Laço 2: Soma com Dependência de Dados
long long soma_dependente = 0;
for (int i = 0; i < TAMANHO_VETOR; i++) {
    soma_dependente += vetor[i];
}

Análise Arquitetónica: Este código cria uma dependência de dados verdadeira (Read-After-Write). A iteração i+1 precisa esperar que a iteração i termine de atualizar a variável soma_dependente. Isso serializa a execução, força paradas no pipeline (stalls) e impede que o processador utilize suas múltiplas unidades de execução em paralelo.

Laço 3: Quebra de Dependência (Fator 4)
long long s1=0, s2=0, s3=0, s4=0;
for (int i = 0; i < TAMANHO_VETOR; i += 4) {
    s1 += vetor[i]; s2 += vetor[i+1]; s3 += vetor[i+2]; s4 += vetor[i+3];
}

Análise Arquitetónica: Ao usar quatro acumuladores independentes, as quatro somas dentro do laço podem ser despachadas para diferentes ALUs e executadas em paralelo no mesmo ciclo de clock. Isso expõe o ILP ao hardware, quebrando a longa cadeia de dependência.

Laço 4: Maximizando o ILP (Fator 8)
long long a1=0, a2=0, a3=0, a4=0, a5=0, a6=0, a7=0, a8=0;
for (int i = 0; i < TAMANHO_VETOR; i += 8) {
    a1 += vetor[i];   a2 += vetor[i+1]; a3 += vetor[i+2]; /*...*/
}

Análise Arquitetónica: Com oito acumuladores, busca-se saturar as unidades de execução do processador. No entanto, o ganho de desempenho tende a diminuir, pois a performance passa a ser limitada por outros fatores do hardware (número de ALUs, largura de banda do pipeline, etc.), um fenómeno conhecido como rendimentos decrescentes.

Procedimentos de Teste
O programa foi compilado e executado para cada nível de otimização do GCC.

Compilação: Compile o código ilp_grafico.c para cada nível de otimização.

# Sem otimização
gcc ilp_grafico.c -o ilp_O0 

# Otimização moderada
gcc -O2 ilp_grafico.c -o ilp_O2

# Otimização agressiva
gcc -O3 ilp_grafico.c -o ilp_O3

Execução e Coleta de Dados: Execute cada versão, passando o nível de otimização como argumento. O script anexa os resultados ao arquivo dados_desempenho.csv.

# Limpa o arquivo CSV antigo
> dados_desempenho.csv
echo "Otimizacao;Laco;Tempo_s" >> dados_desempenho.csv

# Executa os testes
./ilp_O0 O0
./ilp_O2 O2
./ilp_O3 O3

Visualização: Utilize um script (Python, R, Gnuplot) para ler o dados_desempenho.csv e gerar um gráfico de barras comparativo.

Resultados
Os tempos de execução (em segundos) obtidos foram os seguintes:

Tipo de Laço

-O0

-O2

-O3

Laço 1 (Inicialização)

0.494s

0.110s

0.120s

Laço 2 (Com Dependência)

0.220s

0.044s

0.051s

Laço 3 (Fator 4)

0.104s

0.047s

0.047s

Laço 4 (Fator 8)

0.077s

0.038s

0.047s

Análise e Conclusão
A análise dos dados revela uma complexa interação entre o estilo de codificação, as otimizações do compilador e a microarquitetura do processador.

Análise com -O0 (Baseline): Sem otimizações, os resultados seguem a teoria: quebrar a dependência manualmente (Laços 3 e 4) acelera a execução em até 3x, pois a arquitetura out-of-order consegue explorar o paralelismo explícito no código.

Impacto das Otimizações (-O2 e -O3): Com otimizações, todos os laços melhoram drasticamente. O resultado mais intrigante ocorre com -O3, onde o desempenho dos Laços 3 e 4 converge para um valor idêntico (0.047s). Este fenómeno sugere que o compilador ativou a vetorização SIMD, transformando o código em instruções que processam múltiplos dados de uma só vez. Uma vez vetorizado, o unrolling manual torna-se irrelevante, e o desempenho atinge um ponto de saturação ditado pelos limites do hardware.

Conclusão Final: A programação de alto desempenho é uma sinergia. O programador deve escrever código que exponha o paralelismo (evitando dependências), e o compilador deve ser instruído (com flags de otimização) a explorar esse potencial ao máximo, aplicando otimizações avançadas como a vetorização.

Gráfico de Desempenho

