#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define LAYER_SIZE 20


struct neuron{
    float weight;
    float bias;
    float value;
};

struct layer {
    struct neuron neurons[LAYER_SIZE];
};


float derive_neuron(float x, float y){
    return (x - y) * ;
}


void calculate_neuron(struct layer *layer1, struct neuron *neuron) {
    float sum = 0;
    for (int i = 0; i < LAYER_SIZE; i++){
        sum += layer1->neurons[i].value *  layer1->neurons[i].weight;
    }
    sum += neuron->bias;
    neuron->value = (sum <= 0) ? 0 : sum;
}
void calculate_layer(struct layer *prev_layer, struct layer *curr_layer) {
    for (int i = 0; i < LAYER_SIZE; i++) {
        calculate_neuron(prev_layer, &curr_layer->neurons[i]);
    }
}

void backpropagate(){
    for (int i = 0; i < LAYER_SIZE; i++){
        if ()

    }

}

void loss(float *res, float *neurons, float *out) {
    for (int i = 0; i < LAYER_SIZE; i++) {
        out[i] = 0.5 * (((res[i]) - neurons[i]) * (res[i] - neurons[i]));
    }
}

int main(void) {
//    srand(time(NULL));
    printf("Hello, NN!\n");
    struct layer layer1;
    struct layer layer2;
    struct layer output;
    struct layer list[] = {layer1, layer2, output};


    float neurons[20] = {46.34651069414813, 6.798066533839791, 32.99274869936022, 43.20280349639412,
                         25.539418354559384, 33.37133708130474, 28.24414714697972, 2.416162030532681,
                         28.016077847152516, 41.329047753775264, 2.489473268168335, 31.419400806005555,
                         20.339699865233474, 25.074633366380628, 18.342944317599553, 1.9548216217991188,
                         2.210663474703428, 49.234310637818616, 43.49503700241205, 47.0548790545903};
    float res[20];
    float out[20];


    float derivative_output[20];
    for (int i = 0; i < LAYER_SIZE; i++) {

        list[0].neurons[i].weight = (rand() % 100) / 100.0f;
        list[0].neurons[i].value = (rand() % 10) / 10.0f;
        list[0].neurons[i].bias = (rand() % 100) / 100.0f;

        list[1].neurons[i].weight = (rand() % 100) / 100.0f;
        list[1].neurons[i].bias = (rand() % 100) / 100.0f;

        list[2].neurons[i].weight = (rand() % 100) / 100.0f;
        list[2].neurons[i].bias = (rand() % 100) / 100.0f;
    }
    for (int i = 0; i < 2; i++) {
        calculate_layer(&list[i], &list[i + 1]);
        for (int j = 0; j < LAYER_SIZE; j++) {
            printf("Layer %d, Neuron %d: %f\n", i, j, list[i + 1].neurons[j].value);
            if (i == 1){
                res[j] = list[i + 1].neurons[j].value;
            }
        }
    }


    return 0;
}
