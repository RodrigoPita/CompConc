- [x] Escritores tem prioridade (espera os leitores atuais, e leitores novos devem esperar ele)

## Modus Operandi:
- [x] Módulo separado para o padrão leitor/escritor, thread-safe, acessado por mais de uma thread
- [x] O número de leitores e escritores é definido pelo usuário
- [x] para C, usar TAD, com arquivo.h para as assinaturas e .c para implementação (sem main)
- [x] 2 tipos threads: sensores e atuadores.

## Sensores

- [x] Quando medição > 30 escreve no buffer. 
- [x] Medição a cada 1s e simulada por random [25,40].
- [X] Inserir no buffer: medição, id do sensor e id da leitura.
- [x] valor mais antigo pode ser sobreescrito, e segue FIFO.
- [x] O buffer tem tamnho fixo de 60.

## Atuador: 
- [x] possui o mesmo id q o sensor. 
- [x] Faz leitura das medições a cada 2s. 
- [x] Se as 5 últimas forem > 35, emite alerta vermelho. 
- [x] Se entre as 15 últimas, 5 forem > 35, alerta amarelo. 
- [x] Do contrário, normal.
- [x] Deve calcular a média das temperaturas e printar na tela.



