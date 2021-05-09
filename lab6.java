/* Disciplina: Computacao Concorrente */
/* Prof.: Silvana Rossetto */
/* Laboratório: 6 */
/* Codigo: Atividade 5 - Implementar um programa concorrente em Java para incrementar de 1 todos os elementos de um vetor. */
/* -------------------------------------------------------------------*/


import java.lang.reflect.Array;
import java.util.Vector;

//classe que estende Thread e implementa a tarefa de cada thread do programa
class T extends Thread {
    //identificador da thread
    private int id;
    private int[] v;
    private int nThreads;

    //construtor
    public T(int tid, int[] vetor, int N) {
        this.id = tid;
        this.v = vetor;
        this.nThreads = N;
    }

    //metodo main da thread
    public void run() {
        System.out.println("Thread " + this.id + " iniciou!");
        for (int i = this.id; i < v.length; i += this.nThreads) {
            this.v[i]++;
        }
        System.out.println("Thread " + this.id + " terminou!");
    }
}

//classe da aplicacao
class Incrementa {
    private static int nElementos = 10; //numero de elementos no vetor
    public static int[] vetor = new int[nElementos];
    static final int N = 5; //numero de threads (<= nElementos)

    public static void main (String[] args) {
        //reserva espaço para um vetor de threads
        Thread[] threads = new Thread[N];

        //preenche o vetor com 0s
        for(int i = 0; i < 10; i++) {
            vetor[i] = 0;
        }

        //printa o vetor inicial
        System.out.print("Vetor Inicial: ");
        for (int i = 0; i < vetor.length; i++) {
            System.out.print(vetor[i]);
        }
        System.out.println('\n');

        //cria as threads da aplicacao
        for (int i=0; i<threads.length; i++) {
            threads[i] = new T(i, vetor, N);
        }

        //inicia as threads
        for (int i=0; i<threads.length; i++) {
            threads[i].start();
        }

        //espera pelo termino de todas as threads
        for (int i=0; i<threads.length; i++) {
            try { threads[i].join(); } catch (InterruptedException e) { return; }
        }

        //printa o vetor final
        System.out.print("\nVetor final: ");
        for (int i = 0; i < vetor.length; i++) {
            System.out.print(vetor[i]);
        }

    }
}
