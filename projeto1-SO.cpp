#include <bits/stdc++.h>
using namespace std;

struct Processo{
    int tempo_chegada;
    int tempo_pico;
    int id;
};

auto cmp = [](Processo a, Processo b) {
    if (a.tempo_chegada == b.tempo_chegada)
        return a.id < b.id;
    return a.tempo_chegada < b.tempo_chegada;
};

auto cmpSJF = [](Processo a, Processo b) {
    if (a.tempo_pico == b.tempo_pico)
        return a.id > b.id;
    return a.tempo_pico > b.tempo_pico;
};

void chegadaProcessos (int tempo_atual, list<Processo> &proc, queue<Processo> &fila) {

    while (!proc.empty() && tempo_atual >= proc.front().tempo_chegada){
        fila.push(proc.front());
        proc.pop_front();
    }
}

void chegadaProcessosSJF (int tempo_atual, list<Processo> &proc,
priority_queue<Processo, vector<Processo>, decltype(cmpSJF)> &fila) {

    while (!proc.empty() && tempo_atual >= proc.front().tempo_chegada){
        fila.push(proc.front());
        proc.pop_front();
    }
}

int main(int argc, char *argv[]) {

    if (argc != 2) {
        cout<<"Quantidade de argumentos incorreta.\n";
        return 0;
    }

    ifstream file;
    file.open(argv[1]);

    if (!file) {
        cout<<"Arquivo inexistente.\n";
        return 0;
    }

    list<Processo> processos;
    int t_chegada, t_pico, nProcessos = 0, quantum = 2;

    while (file >> t_chegada >> t_pico) {

        Processo p;
        p.tempo_chegada = t_chegada;
        p.tempo_pico = t_pico;
        p.id = nProcessos;

        processos.push_back(p);
        nProcessos++;
    }
    file.close();

    processos.sort(cmp);
    list<Processo> processosFCFS(processos);
    list<Processo> processosSJF(processos);
    list<Processo> processosRR(processos);

    // FILA FCFS
    queue<Processo> filaFCFS;
    vector<vector<int> > temposFCFS(3,vector<int>(nProcessos,-1));
    int tempo_atual = processosFCFS.front().tempo_chegada;
    
    filaFCFS.push(processosFCFS.front());
    processosFCFS.pop_front();
    chegadaProcessos(tempo_atual, processosFCFS, filaFCFS);

    while (!filaFCFS.empty()) {

        int processoAtual = filaFCFS.front().id;
        int t_picoAtual = filaFCFS.front().tempo_pico;
        int t_chegadaAtual = filaFCFS.front().tempo_chegada;
        filaFCFS.pop();

        // Tempo de resposta - FCFS
        temposFCFS[0][processoAtual] = tempo_atual - t_chegadaAtual;
        tempo_atual += t_picoAtual;

        // Tempo de retorno - FCFS
        temposFCFS[1][processoAtual] = tempo_atual - t_chegadaAtual;

        // Tempo de espera - FCFS
        temposFCFS[2][processoAtual] = temposFCFS[1][processoAtual] - t_picoAtual;

        chegadaProcessos(tempo_atual, processosFCFS, filaFCFS);
    }
    
    // FILA SJF
    priority_queue<Processo, vector<Processo>, decltype(cmpSJF)> filaSJF (cmpSJF);
    vector<vector<int> > temposSJF(3,vector<int>(nProcessos,-1));
    tempo_atual = processosSJF.front().tempo_chegada;
    
    filaSJF.push(processosSJF.front());
    processosSJF.pop_front();
    chegadaProcessosSJF(tempo_atual, processosSJF, filaSJF);

    while (!filaSJF.empty()) {

        int processoAtual = filaSJF.top().id;
        int t_picoAtual = filaSJF.top().tempo_pico;
        int t_chegadaAtual = filaSJF.top().tempo_chegada;
        filaSJF.pop();

        // Tempo de resposta - SJF
        temposSJF[0][processoAtual] = tempo_atual - t_chegadaAtual;
        tempo_atual += t_picoAtual;

        // Tempo de retorno - SJF
        temposSJF[1][processoAtual] = tempo_atual - t_chegadaAtual;

        // Tempo de espera - SJF
        temposSJF[2][processoAtual] = temposSJF[1][processoAtual] - t_picoAtual;

        chegadaProcessosSJF(tempo_atual, processosSJF, filaSJF);    
    }

    // FILA RR
    queue<Processo> filaRR;
    vector<vector<int> > temposRR(3,vector<int>(nProcessos,-1));
    tempo_atual = processosRR.front().tempo_chegada;
    
    filaRR.push(processosRR.front());
    processosRR.pop_front();

    while (!filaRR.empty()) {

        chegadaProcessos(tempo_atual, processosRR, filaRR);
        
        int processoAtual = filaRR.front().id;
        int t_picoAtual = filaRR.front().tempo_pico;
        int t_chegadaAtual = filaRR.front().tempo_chegada;
        filaRR.pop();

        // Tempo de resposta - RR
        if (temposRR[0][processoAtual]==-1)
            temposRR[0][processoAtual] = tempo_atual - t_chegadaAtual;

        if (t_picoAtual <= quantum) {
            tempo_atual += t_picoAtual;
            
            // Tempo de retorno - RR
            temposRR[1][processoAtual] = tempo_atual - t_chegadaAtual;
            chegadaProcessos(tempo_atual, processosRR, filaRR);
        }
        else {
            tempo_atual += quantum;
            chegadaProcessos(tempo_atual, processosRR, filaRR);

            Processo p;
            p.id = processoAtual;
            p.tempo_pico = t_picoAtual-quantum;
            p.tempo_chegada = t_chegadaAtual;
            filaRR.push(p);
        }
    }

    // Cálculo tempo de espera - RR
    for (int i=0;i<nProcessos;i++){
        Processo p;
        p.id = processos.front().id;
        p.tempo_pico = processos.front().tempo_pico;

        temposRR[2][p.id] = temposRR[1][p.id] - p.tempo_pico;
        processos.push_back(p);
        processos.pop_front();
    }

    // Cálculo das médias dos tempos de retorno, resposta
    // e espera para cada algoritmo de escalonamento

    double mediaRetorno[3] = {0,0,0},
           mediaResposta[3] = {0,0,0},
           mediaEspera[3] = {0,0,0};

    for (int i=0;i<nProcessos;i++) {
        mediaRetorno[0] += temposFCFS[1][i];
        mediaRetorno[1] += temposSJF[1][i];
        mediaRetorno[2] += temposRR[1][i];

        mediaResposta[0] += temposFCFS[0][i];
        mediaResposta[1] += temposSJF[0][i];
        mediaResposta[2] += temposRR[0][i];

        mediaEspera[0] += temposFCFS[2][i];
        mediaEspera[1] += temposSJF[2][i];
        mediaEspera[2] += temposRR[2][i];
    }

    for (int i=0;i<3;i++){
        mediaRetorno[i] /= nProcessos;
        mediaResposta[i] /= nProcessos;
        mediaEspera[i] /= nProcessos;
    }

    cout << fixed<< setprecision(1);
    cout << "FCFS "<< mediaRetorno[0] << " "<< mediaResposta[0]<< " "<< mediaEspera[0] << "\n";
    cout << "SJF "<< mediaRetorno[1] << " "<< mediaResposta[1]<< " "<< mediaEspera[1] << "\n";
    cout << "RR "<< mediaRetorno[2] << " "<< mediaResposta[2]<< " "<< mediaEspera[2] << "\n";

    return 0;
}