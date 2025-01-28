#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


const float learning_rate = 0.1;
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


void change_output(struct layer *curr_layer, float *res, float *neurons, struct layer *prev_layer) {
    float error, gradient;
    for (int j = 0; j < curr_layer -> size; j++) {
        error = res[j] - neurons[j];
        for (int i = 0; i < prev_layer -> size; i++) {
            gradient = error * prev_layer->neurons[i].value;
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
            float result = sum(curr_layer -> neurons[i].output_gradient, 20);
            for (int j = 0; j < prev_layer -> size; j++) {
                gradient = result * prev_layer->neurons[j].value;
                prev_layer->neurons[j].input_weights[i] -= gradient * learning_rate;
                prev_layer->neurons[j].output_gradient[i] = gradient;

            }
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


void loss(float *res, float *neurons, float *out) {
    for (int i = 0; i < 5; i++) {
        out[i] = 0.5 * ((res[i] - neurons[i]) * (res[i] - neurons[i]));
    }
}

int main(void) {
    printf("Hello, NN!\n");

    struct layer input, hidden1, hidden2, output;
    input.size = 10;
    hidden1.size = 32;
    hidden2.size = 32;
    output.size = 1;
    struct layer list[] = {input, hidden1, hidden2, output};

    float neurons[output.size];
    for (int i = 0; i < output.size; i++) {
        neurons[i] = (float)(i + 1);
    }
    float res[20];
    float out[20];

    int list_len = sizeof(list)/sizeof(struct layer);
    // create layers
    for (int l = 0; l < list_len; l++) {
        list[l].neurons = (struct neuron *)malloc(list[l].size * sizeof(struct neuron));
        if (list[l].neurons == NULL) {
            printf("Memory allocation failed for neurons!\n");
            exit(1);
        }
        printf("Layer %d:\n", list[l].size);
        for (int i = 0; i < list[l].size; i++) {
            list[l].neurons[i].input_weights = (float*) malloc(list[l + 1].size * sizeof(float));
            if (list[l].neurons[i].input_weights == NULL) {
                printf("Memory allocation failed for neurons!\n");
                exit(1);
            }
            list[l].neurons[i].output_gradient = (float*) malloc(list[l].size * sizeof(float));
            if (list[l].neurons[i].output_gradient == NULL) {
                printf("Memory allocation failed for neurons!\n");
                exit(1);
            }
            list[l].neurons[i].value = (rand() % 10) / 10.0f;
            list[l].neurons[i].bias = (rand() % 100) / 100.0f;
            if (l != list_len - 1) {
            for (int j = 0; j < list[l + 1].size; j++) {
                list[l].neurons[i].input_weights[j] = (rand() % 100) / 100.0f;
            }
            }
        }
    }



    for (int epoch = 0; epoch < epochs; epoch++) {
        for (int i = 0; i < 3; i++) {
            calculate_layer(&list[i], &list[i + 1]);
            calculate_layer(&list[i + 1], &list[i + 2]);
            for (int j = 0; j < list[i + 1].size; j++) {
                if (i == 1) {
                    res[j] = list[i + 1].neurons[j].value;
                }
            }

            loss(res, neurons, out);

            change_output(&list[3], res, neurons, &list[2]);
            change_layer(&list[2], &list[1]);
            change_layer(&list[1], &list[0]);

            float total_loss = 0;
            for (int i = 0; i < list[3].size; i++) {
                total_loss += out[i];
            }

            printf("Epoch %d, Loss: %f\n", epoch, total_loss);

            printf("Epoch %d\n", epoch);
            for (int i = 0; i < list[3].size; i++) {
                printf("Output: %.4f\tExpected: %.4f\n", list[3].neurons[i].value, neurons[i]);
            }
        }
    }
    for (int l = 0; l < sizeof(list)/sizeof(struct layer); l++) {
        for (int i = 0; i < list[l].size; i++) {
            free(list[l].neurons[i].input_weights);
            free(list[l].neurons[i].output_gradient);
        }
        free(list[l].neurons);
    }
    return 0;
}