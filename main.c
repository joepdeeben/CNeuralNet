#include <stdio.h>
#include <stdlib.h>
#include <tgmath.h>
#include <stdbool.h>
const float learning_rate = 0.005;
const float div_loss_weight = 5.0f;
const int epochs = 20000;

struct neuron {
    float *input_weights;
    float delta;
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


void change_output(struct layer *curr_layer, float *neurons, struct layer *prev_layer, float loss_weight) {
    float error, gradient;

    for (int i = 0; i < prev_layer->size; i++) {
        prev_layer->neurons[i].delta = 0.0f;
    }

    // j neuron in current layer
    for (int j = 0; j < curr_layer -> size; j++) {
        error = (curr_layer -> neurons[j].value - neurons[j]) * loss_weight;
        // i neuron in previous layer
        for (int i = 0; i < prev_layer -> size; i++) {
            // calulate gradient through derivative
            gradient = error * prev_layer->neurons[i].value;
            // update corresponding weight with gradient
            // accumulate delta for the previous neuron
            prev_layer->neurons[i].delta += error * curr_layer->neurons[j].input_weights[i];
            curr_layer->neurons[j].input_weights[i] -= gradient * learning_rate;
        }
        // update bias
        curr_layer->neurons[j].bias -= error * learning_rate;
    }
}

void change_layer(struct layer *curr_layer, struct layer *prev_layer, struct layer *next_layer) {
    float gradient;
    // calc gradient len

    for (int j = 0; j < prev_layer->size; j++) {
        prev_layer->neurons[j].delta = 0.0f;
    }

    // i neuron in current layer
    for (int i = 0; i < curr_layer -> size; i++) {;
        // relu derivative
        if (curr_layer->neurons[i].value > 0) {
            // sum error for neuron
            float delta = curr_layer->neurons[i].delta;
            // j neuron in prev layer
            for (int j = 0; j < prev_layer -> size; j++) {
                // calulate gradient through derivative
                gradient = delta * prev_layer->neurons[j].value;
                // update corresponding weight with gradient
                // accumulate delta for the previous neuron
                prev_layer->neurons[j].delta += curr_layer->neurons[i].input_weights[j] * delta;
                curr_layer->neurons[i].input_weights[j] -= gradient * learning_rate;
                

            }
            curr_layer->neurons[i].bias -= delta * learning_rate;


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
                for (int k = 0; k < layers[i - 1]; k++) {
                        layers_arr[i].neurons[j].input_weights[k] = ((rand() % 200) - 100) / (sqrt(layers[i - 1]) * 100.0f);;
                    }
	            } else {

                layers_arr[i].neurons[j].input_weights = NULL;
            }


            layers_arr[i].neurons[j].value = (rand() % 10) / 10.0f;
            layers_arr[i].neurons[j].bias = (rand() % 100) / 100.0f;
            layers_arr[i].neurons[j].delta = 0.0f;
        }
    }
    return layers_arr;
}



static inline int op_to_int(char op) {
    switch (op) {
        case '+': return 0;
        case '-': return 1;
        case '*': return 2;
        case '/': return 3;
        default:  return 0;
    }
}

static inline char int_to_op(int code) {
    switch (code) {
        case 0: return '+';
        case 1: return '-';
        case 2: return '*';
        case 3: return '/';
        default:return '?';
    }
}


int main(void) {
    printf("Hello, NN!\n");

    int layers[] = {6, 320, 160, 160, 1};
    int layer_n = sizeof(layers) / sizeof(int);

    struct layer *list = initialize_layers(layers, layer_n);
    printf("Layers created\n");

        const int N_SAMPLES = 100;

    float expected[1];

    for (int epoch = 0; epoch < epochs; epoch++) {
    float epoch_loss = 0.0f;

    for (int s = 0; s < N_SAMPLES; s++) {
        // --- sample arithmetic problem ---
        int a = (rand() % 21) - 10;   // [-10, 10]
        int b = (rand() % 21) - 10;   // [-10, 10]
        int op_code = rand() % 4;
        char op = int_to_op(op_code);

        // avoid division by zero
        if (op == '/' && b == 0) b = 1;

        // compute target as float
        float y;
        switch (op) {
            case '+': y = (float)(a + b); break;
            case '-': y = (float)(a - b); break;
            case '*': y = (float)(a * b); break;
            case '/': y = (float)a / (float)b; break;
            default:  y = 0.0f; break;
        }

        // --- encode inputs as floats ---
        // layout: [a, b, op+, op-, op*, op/]
        float input[6] = {(float)a, (float)b, 0.0f, 0.0f, 0.0f, 0.0f};
        input[2 + op_code] = 1.0f;

        for (int i = 0; i < 6; i++) {
            list[0].neurons[i].value = input[i];
        }

        float expected[1];
        expected[0] = y / 100.0f; // scale output for mixed ops

        //------------------------------------------
        // 1) forward pass
        //------------------------------------------
        for (int i = 0; i < layer_n - 1; i++) {
            bool is_last = (i == layer_n - 2);
            calculate_layer(&list[i], &list[i + 1], is_last);
        }

        //------------------------------------------
        // 2) backward pass
        //------------------------------------------
        float loss_weight = (op == '/') ? div_loss_weight : 1.0f;
        change_output(&list[layer_n - 1], expected, &list[layer_n - 2], loss_weight);
        for (int i = layer_n - 2; i > 0; i--) {
            change_layer(&list[i], &list[i - 1], &list[i + 1]);
        }

        //------------------------------------------
        // 3) accumulate loss
        //------------------------------------------
        epoch_loss += loss_weight * loss(&list[layer_n - 1], expected);
    }

    epoch_loss /= N_SAMPLES;
    printf("Epoch %d, Avg Loss: %f\n", epoch, epoch_loss);

    // --- one fresh sample for sanity print ---
    int a = (rand() % 21) - 10;
    int b = (rand() % 21) - 10;
    int op_code = rand() % 4;
    char op = int_to_op(op_code);
    if (op == '/' && b == 0) b = 1;

    float y;
    switch (op) {
        case '+': y = (float)(a + b); break;
        case '-': y = (float)(a - b); break;
        case '*': y = (float)(a * b); break;
        case '/': y = (float)a / (float)b; break;
        default:  y = 0.0f; break;
    }

    // layout: [a, b, op+, op-, op*, op/]
    float input[6] = {(float)a, (float)b, 0.0f, 0.0f, 0.0f, 0.0f};
    input[2 + op_code] = 1.0f;

    for (int i = 0; i < 6; i++) {
        list[0].neurons[i].value = input[i];
    }

    for (int i = 0; i < layer_n - 1; i++) {
        bool is_last = (i == layer_n - 2);
        calculate_layer(&list[i], &list[i + 1], is_last);
    }

    float pred = list[layer_n - 1].neurons[0].value * 100.0f; // unscale
    printf("Test: %d %c %d = %.4f | pred = %.4f\n", a, op, b, y, pred);
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
