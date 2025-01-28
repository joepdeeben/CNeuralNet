#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>


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
            prev_layer->neurons[i].input_weights[j] -= gradient * learning_rate;
            prev_layer->neurons[i].output_gradient[j] = gradient;
        }
        curr_layer->neurons[j].bias -= error * learning_rate;
    }
}

void change_layer(struct layer *curr_layer, struct layer *prev_layer, struct layer *next_layer) {
    float gradient;
    for (int i = 0; i < curr_layer -> size; i++) {
        if (curr_layer->neurons[i].value > 0) {
            float result = sum(curr_layer -> neurons[i].output_gradient, next_layer-> size);
            for (int j = 0; j < prev_layer -> size; j++) {
                gradient = result * prev_layer->neurons[j].value;
                prev_layer->neurons[j].input_weights[i] -= gradient * learning_rate;
                prev_layer->neurons[j].output_gradient[i] = gradient;

            }
            curr_layer->neurons[i].bias -= result * learning_rate;
        }
    }
}

void calculate_neuron(struct layer *prev_layer, struct neuron *neuron, int index, bool is_output_layer) {
    float sum = 0;
    for (int i = 0; i < prev_layer -> size; i++) {
        sum += prev_layer->neurons[i].value * prev_layer->neurons[i].input_weights[index];
    }
    sum += neuron->bias;
    if (is_output_layer) {
        neuron->value = sum;
    } else {
        neuron->value = (sum <= 0) ? 0.01f * sum: sum;
    }
}

void calculate_layer(struct layer *prev_layer, struct layer *curr_layer, bool is_output_layer) {
    for (int i = 0; i < curr_layer -> size; i++) {
        calculate_neuron(prev_layer, &curr_layer->neurons[i], i, is_output_layer);
    }
}

void calculate_network(struct layer *all_layers, int size) {
    for (int i = 0; i < size - 1; i++) {
        bool is_output_layer = (i == size - 2);
        calculate_layer(&all_layers[i], &all_layers[i + 1], is_output_layer);
    }
}

void backprop(struct layer *all_layers, int amount_layers, float *res, float *expected) {

    change_output(&all_layers[amount_layers - 1], res, expected,  &all_layers[amount_layers - 2]);

    for (int i = amount_layers - 2; i > 0 ; i--) {
        change_layer(&all_layers[i], &all_layers[i - 1], &all_layers[i +1]);
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
        layers_arr[i].size = layers[i];
        if (layers_arr[i].neurons == NULL) {
            printf("Error allocating neuron\n");
            for (int i = 0; i < size; i++) {
                free(layers_arr[i].neurons);
            }
            return NULL;
        }
        for (int j = 0; j < layers[i]; j++) {
            if (i < size - 1){
            layers_arr[i].neurons[j].input_weights = (float*) malloc(layers[i + 1] * sizeof(float));
            if (layers_arr[i].neurons[j].input_weights == NULL) {
                printf("Error allocating input weights\n");
                for (int i = 0; i < size; i++) {
                    free(layers_arr[i].neurons[j].input_weights);
                }

            }
            layers_arr[i].neurons[j].output_gradient = (float *)malloc(layers[i + 1] * sizeof(float));
            if (layers_arr[i].neurons[j].output_gradient == NULL) {
                printf("Error allocating output gradients\n");
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

void run_epoch(int epochs, struct layer *layer, int amount_layers, float *expected) {
        for (int i = 0; i < epochs; i++) {
            printf("epoch %d\n", i);
            calculate_network(layer, amount_layers);

            int output_size = layer[amount_layers - 1].size;
            float *res = malloc(sizeof(float) * output_size);
            float *out = malloc(sizeof(float) * output_size);
            if (res == NULL || out == NULL) {
                printf("Memory allocation failed\n");
                return;
            }
            for (int j = 0; j < output_size; j++) {
                res[j] = layer[amount_layers - 1].neurons[j].value;
            }
            loss(res, expected, out, output_size);
            for (int k = 0; k < output_size; k++) {
                printf("actual: %f expected: %f\n", out[k], expected[k]);
            }

            backprop(layer, amount_layers, res, expected);
            free(res);
            free(out);
        }

}

int main(void) {
    printf("Hello, NN!\n");



    int layers[] = {20, 20, 20};
    int size = sizeof(layers) / sizeof(int);

    struct layer *layer = initialize_layers(layers, size);


    float *expected = malloc(layers[size - 1] * sizeof(float));
    if (expected == NULL) {
        printf("Memory allocation failed for expected\n");
        return -1;
    }

    for (int i = 0; i < layers[size - 1]; i++) {
        expected[i] = (float)(i + 1);
    }



    run_epoch(epochs, layer, size, expected);
    printf("after");


    return 0;
}