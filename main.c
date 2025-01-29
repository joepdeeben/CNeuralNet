#include <stdio.h>
#include <stdlib.h>
#include <tgmath.h>
#include <stdbool.h>
const float learning_rate = 0.05;
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


void change_output(struct layer *curr_layer, float *neurons, struct layer *prev_layer) {
    float error, gradient;

    // j neuron in current layer
    for (int j = 0; j < curr_layer -> size; j++) {
        error = curr_layer -> neurons[j].value - neurons[j];
        // i neuron in previous layer
        for (int i = 0; i < prev_layer -> size; i++) {
            // calulate gradient through derivative
            gradient = error * prev_layer->neurons[i].value;
            // update corresponding weight with gradient
            // store gradient per weight in previous neuron to make error per neuron easier
            curr_layer->neurons[j].input_weights[i] -= gradient * learning_rate;
            prev_layer->neurons[i].output_gradient[j] = gradient;
        }
        // update bias
        curr_layer->neurons[j].bias -= error * learning_rate;
    }
}

void change_layer(struct layer *curr_layer, struct layer *prev_layer, struct layer *next_layer) {
    float gradient;
    // calc gradient len
    int grad_len = next_layer -> size;
    // i neuron in current layer
    for (int i = 0; i < curr_layer -> size; i++) {;
        // relu derivative
        if (curr_layer->neurons[i].value > 0) {
            // sum error for neuron
            float result = sum(curr_layer -> neurons[i].output_gradient, grad_len);
            // j neuron in prev layer
            for (int j = 0; j < prev_layer -> size; j++) {
                // calulate gradient through derivative
                gradient = (result / grad_len) * prev_layer->neurons[j].value;
                // update corresponding weight with gradient
                // store gradient per weight in previous neuron to make error per neuron easier
                curr_layer->neurons[i].input_weights[j] -= gradient * learning_rate;
                prev_layer->neurons[j].output_gradient[i] = gradient;

            }
            curr_layer->neurons[i].bias -= (result / grad_len) * learning_rate;


        }

    }

}

void calculate_neuron(struct layer *prev_layer, struct neuron *neuron, bool is_last_layer) {
    float sum = 0;

    for (int i = 0; i < prev_layer->size; i++) {
        sum += prev_layer->neurons[i].value * neuron->input_weights[i];
    }
    sum += neuron->bias;


    if (!is_last_layer) {
        neuron->value = (sum <= 0) ? 0 : sum;
    } else {
        neuron->value = sum;
    }
}

void calculate_layer(struct layer *prev_layer, struct layer *curr_layer, bool is_last_layer) {
    for (int i = 0; i < curr_layer->size; i++) {
        calculate_neuron(prev_layer, &curr_layer->neurons[i], is_last_layer);
    }
}

float loss(struct layer *res, float *expected) {
    float s = 0.0f;
    int len = res->size;
    for (int i = 0; i < len; i++) {
        float output = res->neurons[i].value;
        float diff = output - expected[i];
        s += 0.5f * diff * diff;
    }
    return s / len;
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
            if (i > 0){
                layers_arr[i].neurons[j].input_weights = malloc(layers[i - 1] * sizeof(float));
                if (layers_arr[i].neurons[j].input_weights == NULL) {
                    printf("Error allocating input weights\n");
                    for (int i = 0; i < size; i++) {
                        free(layers_arr[i].neurons[j].input_weights);
                    }

                }
                if (i < size - 1) {
                    layers_arr[i].neurons[j].output_gradient = (float *)malloc(layers[i + 1] * sizeof(float));
                    if (layers_arr[i].neurons[j].output_gradient == NULL) {
                        printf("Error allocating output gradients\n");
                    }
                }

                for (int k = 0; k < layers[i - 1]; k++) {
                        layers_arr[i].neurons[j].input_weights[k] = ((rand() % 200) - 100) / (sqrt(layers[i - 1]) * 100.0f);;
                    }
            } else {

                layers_arr[i].neurons[j].input_weights = NULL;
                layers_arr[i].neurons[j].output_gradient = (float *)malloc(layers[i + 1] * sizeof(float));
                if (layers_arr[i].neurons[j].output_gradient == NULL) {
                    printf("Error allocating output gradients\n");
                }

            }


            layers_arr[i].neurons[j].value = (rand() % 10) / 10.0f;
            layers_arr[i].neurons[j].bias = (rand() % 100) / 100.0f;
        }
    }
    return layers_arr;
}

int main(void) {
    printf("Hello, NN!\n");

    int layers[] = {5, 640, 320, 160, 5};
    int layer_n = sizeof(layers) / sizeof(int);

    struct layer *list = initialize_layers(layers, layer_n);
    printf("Layers created\n");

    float expected[20];
    for (int i = 0; i < layers[layer_n - 1]; i++) {
        expected[i] = (float)(i + 1);
    }
    float res[20];

    for (int epoch = 0; epoch < epochs; epoch++) {
        //------------------------------------------
        // 1) forward pass from input to output
        //------------------------------------------
        for (int i = 0; i < layer_n - 1; i++) {
            bool is_last = (i == layer_n - 2);  // Last layer before output
            calculate_layer(&list[i], &list[i + 1], is_last);
        }


        //------------------------------------------
        // 2) Compute the error and do the backward pass
        //------------------------------------------
        change_output(&list[layer_n - 1], expected, &list[layer_n - 2]);

        for (int  i = layer_n - 2; i > 0; i--) {
            change_layer(&list[i], &list[i - 1], &list[i + 1]);
        }


        //------------------------------------------
        // 3) Compute and print loss
        //------------------------------------------
        float total_loss = loss(&list[layer_n - 1], expected);
        printf("Epoch %d, Loss: %f\n", epoch, total_loss);
        for (int i = 0; i < layers[layer_n - 1]; i++) {
            printf("Output: %.4f\tExpected: %.4f\n", list[layer_n - 1].neurons[i].value, expected[i]);
        }
    }

    for (int layer = 0; layer < layer_n; layer++) {
        for (int i = 0; i < layers[layer]; i++) {
            free(list[layer].neurons[i].input_weights);
        }
        free(list[layer].neurons);
    }
    free(list);

    return 0;
}