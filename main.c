#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


const float learning_rate = 0.001;
const int epochs = 100;

struct neuron {
    float *input_weights;
    float *output_gradient;
    float bias;
    float value;
};

struct layer {
    struct neuron *neurons;
    int size;
};



float sum(float arr[], int size) {
    float total = 0.0;
    for (int i = 0; i < size; i++) {
        total += arr[i];
    }
    return total;
}

float clip(float n, float lower, float upper) {
    if (n < lower) {
        return lower;
    } if (n > upper) {
        return upper;
    }
}


void change_output(struct layer *curr_layer, float *res, float *expected, struct layer *prev_layer) {
    float error, gradient;
    for (int j = 0; j < curr_layer -> size; j++) {
        error = res[j] - expected[j];
        for (int i = 0; i < prev_layer -> size; i++) {
            gradient = error * prev_layer->neurons[i].value;
            // gradient = clip(gradient, -10.0, 10.0);
            prev_layer->neurons[i].input_weights[j] -= gradient * learning_rate;
            prev_layer->neurons[i].output_gradient[j] = gradient;
        }
        curr_layer->neurons[j].bias -= error * learning_rate;
    }
}

void change_layer(struct layer *curr_layer, struct layer *prev_layer) {
    float gradient;
    for (int i = 0; i < curr_layer -> size; i++) {
        if (curr_layer->neurons[i].value > 0) {
            float result = sum(curr_layer -> neurons[i].output_gradient, curr_layer -> size);
            for (int j = 0; j < prev_layer -> size; j++) {
                gradient = result * prev_layer->neurons[j].value;
                // gradient = clip(gradient, -10.0, 10.0);
                prev_layer->neurons[j].input_weights[i] -= gradient * learning_rate;
                prev_layer->neurons[j].output_gradient[i] = gradient;

            }
            curr_layer->neurons[i].bias -= result * learning_rate;
        }
    }
}

void calculate_neuron(struct layer *curr_layer, struct layer *prev_layer, struct neuron *neuron, int index) {
    float sum = 0;
    for (int i = 0; i < prev_layer -> size; i++) {
        sum += prev_layer->neurons[i].value * prev_layer->neurons[i].input_weights[index];
    }
    sum += neuron->bias;
    neuron->value = (sum <= 0) ? sum * 0.01 : sum;
}

void calculate_layer(struct layer *prev_layer, struct layer *curr_layer) {
    for (int i = 0; i < curr_layer -> size; i++) {
        calculate_neuron(curr_layer, prev_layer, &curr_layer->neurons[i], i);
    }
}


void loss(float *res, float *expected, float *out, int size) {
    for (int i = 0; i < size; i++) {
        out[i] = 0.5 * ((res[i] - expected[i]) * (res[i] - expected[i]));
    }
}

struct layer * initialize_layers(int *layers, int size) {
    struct layer *layers_arr = malloc(sizeof(struct layer) * size);
    if (layers_arr == NULL) {
        printf("Error allocating layer\n");
        return NULL;
    }
    for (int i = 0; i < size; i++) {
        layers_arr[i].neurons = malloc(sizeof(struct neuron) * layers[i]);
        if (layers_arr[i].neurons == NULL) {
            printf("Error allocating neuron\n");
            free(layers_arr[i].neurons);
            return NULL;
        }
        for (int j = 0; j < layers[i]; j++) {
            if (i < size - 1){
            layers_arr[i].neurons[j].input_weights = (float*) malloc(layers[i + 1] * sizeof(float));
            if (layers_arr[i].neurons[j].input_weights == NULL) {
                printf("Error allocating input weights\n");
                free(layers_arr[i].neurons[j].input_weights);
            }

            for (int k = 0; k < layers[i + 1]; k++) {
                layers_arr[i].neurons[j].input_weights[k] = (rand() % 100) / 100.0f;
            }
            } else {

                layers_arr[i].neurons[j].input_weights = NULL;
            }


            layers_arr[i].neurons[j].value = (rand() % 10) / 10.0f;
            layers_arr[i].neurons[j].bias = (rand() % 100) / 100.0f;
        }
    }
    return layers_arr;
}


int main(void) {
    printf("Hello, NN!\n");



    int layers[] = {10, 20, 10, 5};
    int size = sizeof(layers) / sizeof(int);

    initialize_layers(layers, size);




    //
    // for (int epoch = 0; epoch < epochs; epoch++) {
    //     for (int i = 0; i < 2; i++) {
    //         calculate_layer(&list[i], &list[i + 1]);
    //         for (int j = 0; j < LAYER_SIZE; j++) {
    //             if (i == 1) {
    //                 res[j] = list[i + 1].neurons[j].value;
    //             }
    //         }
    //
    //         loss(res, neurons, out);
    //
    //         change_output(&list[2], res, neurons, &list[1]);
    //         change_layer(&list[1], &list[0], &list[2], res, neurons);
    //
    //         float total_loss = 0;
    //         for (int i = 0; i < LAYER_SIZE; i++) {
    //             total_loss += out[i];
    //         }
    //
    //         printf("Epoch %d, Loss: %f\n", epoch, total_loss);
    //
    //         printf("Epoch %d\n", epoch);
    //         for (int i = 0; i < LAYER_SIZE; i++) {
    //             printf("Output: %.4f\tExpected: %.4f\n", list[2].neurons[i].value, neurons[i]);
    //         }
    //     }
    // }
    //
    // for (int i = 0; i < LAYER_SIZE; i++) {
    //     free(list[0].neurons[i].input_weights);
    //     free(list[1].neurons[i].input_weights);
    //     free(list[2].neurons[i].input_weights);
    // }

    return 0;
}