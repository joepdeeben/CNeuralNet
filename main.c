#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define LAYER_SIZE 20
const float learning_rate = 0.1;
const int epochs = 100;

struct neuron {
    float *input_weights;
    float output_gradient[20];
    float bias;
    float value;
};

struct layer {
    struct neuron neurons[LAYER_SIZE];
};



float sum(float arr[], int size) {
    float total = 0.0;
    for (int i = 0; i < size; i++) {
        total += arr[i];
    }
    return total;
}


void change_output(struct layer *curr_layer, float *res, float *neurons, struct layer *prev_layer) {
    float error, gradient;
    for (int j = 0; j < LAYER_SIZE; j++) {
        error = res[j] - neurons[j];
        for (int i = 0; i < LAYER_SIZE; i++) {
            gradient = error * prev_layer->neurons[i].value;
            prev_layer->neurons[i].input_weights[j] -= gradient * learning_rate;
            prev_layer->neurons[i].output_gradient[j] = gradient;
        }
        curr_layer->neurons[j].bias -= error * learning_rate;
    }
}

void change_layer(struct layer *curr_layer, struct layer *prev_layer, struct layer *next_layer, float *res, float *neurons) {
    float gradient;
    for (int i = 0; i < LAYER_SIZE; i++) {
        if (curr_layer->neurons[i].value > 0) {
            float result = sum(curr_layer -> neurons[i].output_gradient, 20);
            for (int j = 0; j < LAYER_SIZE; j++) {
                gradient = result * prev_layer->neurons[j].value;
                prev_layer->neurons[j].input_weights[i] -= gradient * learning_rate;

            }
        }
    }
}

void calculate_neuron(struct layer *layer1, struct neuron *neuron) {
    float sum = 0;
    for (int i = 0; i < LAYER_SIZE; i++) {
        sum += layer1->neurons[i].value * neuron->input_weights[i];
    }
    sum += neuron->bias;
    neuron->value = (sum <= 0) ? 0 : sum;
}

void calculate_layer(struct layer *prev_layer, struct layer *curr_layer) {
    for (int i = 0; i < LAYER_SIZE; i++) {
        calculate_neuron(prev_layer, &curr_layer->neurons[i]);
    }
}

void loss(float *res, float *neurons, float *out) {
    for (int i = 0; i < LAYER_SIZE; i++) {
        out[i] = 0.5 * ((res[i] - neurons[i]) * (res[i] - neurons[i]));
    }
}

int main(void) {
    printf("Hello, NN!\n");

    struct layer layer1, layer2, output;
    struct layer list[] = {layer1, layer2, output};

    float neurons[20];
    for (int i = 0; i < LAYER_SIZE; i++) {
        neurons[i] = (float)(i + 1);
    }
    float res[20];
    float out[20];

    for (int i = 0; i < LAYER_SIZE; i++) {
        list[0].neurons[i].input_weights = (float*) malloc(LAYER_SIZE * sizeof(float));
        list[0].neurons[i].value = (rand() % 10) / 10.0f;
        list[0].neurons[i].bias = (rand() % 100) / 100.0f;

        for (int j = 0; j < LAYER_SIZE; j++) {
            list[0].neurons[i].input_weights[j] = (rand() % 100) / 100.0f;
        }

        list[1].neurons[i].input_weights = (float*) malloc(LAYER_SIZE * sizeof(float));
        list[1].neurons[i].bias = (rand() % 100) / 100.0f;

        for (int j = 0; j < LAYER_SIZE; j++) {
            list[1].neurons[i].input_weights[j] = (rand() % 100) / 100.0f;
        }

        list[2].neurons[i].input_weights = (float*) malloc(LAYER_SIZE * sizeof(float));
        list[2].neurons[i].bias = (rand() % 100) / 100.0f;

        for (int j = 0; j < LAYER_SIZE; j++) {
            list[2].neurons[i].input_weights[j] = (rand() % 100) / 100.0f;
        }
    }



    for (int epoch = 0; epoch < epochs; epoch++) {
        for (int i = 0; i < 2; i++) {
            calculate_layer(&list[i], &list[i + 1]);
            for (int j = 0; j < LAYER_SIZE; j++) {
                if (i == 1) {
                    res[j] = list[i + 1].neurons[j].value;
                }
            }

            loss(res, neurons, out);

            change_output(&list[2], res, neurons, &list[1]);
            change_layer(&list[1], &list[0], &list[2], res, neurons);

            float total_loss = 0;
            for (int i = 0; i < LAYER_SIZE; i++) {
                total_loss += out[i];
            }

            printf("Epoch %d, Loss: %f\n", epoch, total_loss);

            printf("Epoch %d\n", epoch);
            for (int i = 0; i < LAYER_SIZE; i++) {
                printf("Output: %.4f\tExpected: %.4f\n", list[2].neurons[i].value, neurons[i]);
            }
        }
    }

    for (int i = 0; i < LAYER_SIZE; i++) {
        free(list[0].neurons[i].input_weights);
        free(list[1].neurons[i].input_weights);
        free(list[2].neurons[i].input_weights);
    }

    return 0;
}
