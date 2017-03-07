#include <fann.h>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <math.h>

using namespace std;

void trainNetwork (void) {
	const float desired_error = 1e-3;
	const unsigned int max_epochs = 300;
	const unsigned int epochs_between_reports = 10;
	struct fann *ann;
	struct fann_train_data *train_data, *test_data;

	printf("Creating network.\n");
	train_data = fann_read_train_from_file("bias.train");
	ann = fann_create_standard(4, train_data->num_input, 100, 10, train_data->num_output);
	fann_set_activation_function_hidden(ann, FANN_SIGMOID_SYMMETRIC_STEPWISE);
	fann_set_activation_function_output(ann, FANN_SIGMOID_SYMMETRIC_STEPWISE);
	fann_set_training_algorithm(ann, FANN_TRAIN_RPROP);
	test_data = fann_read_train_from_file("bias.test");

	cout << "Total neurons: " << fann_get_total_neurons(ann) << endl;
	cout << "Input neurons: " << fann_get_num_input(ann) << endl;
	cout << "Output neurons: " << fann_get_num_output(ann) << endl;
	
	printf("Training network.\n");
	ofstream out;
	out.open("bias.err");
	for (int i=1; i<=max_epochs; i++) {
		cout << i << "\t";
		fann_train_on_data(ann, train_data, 1, epochs_between_reports, desired_error);
		fann_reset_MSE(ann);
		for(int i = 0; i < fann_length_train_data(train_data); i++)
			fann_test(ann, train_data->input[i], train_data->output[i]);
		double train_error = fann_get_MSE(ann);
		fann_reset_MSE(ann);
		for(int i = 0; i < fann_length_train_data(test_data); i++)
			fann_test(ann, test_data->input[i], test_data->output[i]);
		double test_error = fann_get_MSE(ann);
		out << "\t" << train_error << "\t\t" << test_error << endl;
		
		if(train_error < desired_error) {
			cout << "Error threshold reached: " << train_error << " < " << desired_error << endl;
			break;
		}
	}
	out.close();
	
	//fann_train_on_data(ann, train_data, max_epochs, epochs_between_reports, desired_error);
	fann_reset_MSE(ann);
	for(int i = 0; i < fann_length_train_data(train_data); i++)
		fann_test(ann, train_data->input[i], train_data->output[i]);
	printf("MSE error on training set: %f\n", fann_get_MSE(ann));

	printf("Testing network.\n");
	test_data = fann_read_train_from_file("bias.test");
	fann_reset_MSE(ann);
	for(int i = 0; i < fann_length_train_data(test_data); i++)
		fann_test(ann, test_data->input[i], test_data->output[i]);
	printf("MSE error on test set: %f\n", fann_get_MSE(ann));

	out.open("test.out");
	cout << "Evaluating on the test set" << endl;
	cout << fixed;
	for(int i = 0; i < test_data->num_data; i++) {
		fann_type* output;
		output = fann_run(ann, test_data->input[i]);
		//cout << *output << "\t\t" << test_data->output[i][0] << "\t\t" << fabs(*output-test_data->output[i][0]) << endl;
		out << *output << "\t\t" << test_data->output[i][0] << endl;
	}
	out.close();
	
	out.open("train.out");
	cout << "Evaluating on the training set" << endl;
	for(int i = 0; i < train_data->num_data; i++) {
		fann_type* output;
		output = fann_run(ann, train_data->input[i]);
		//cout << *output << "\t\t" << train_data->output[i][0] << "\t\t" << fabs(*output-train_data->output[i][0]) << endl;
		out << *output << "\t\t" << train_data->output[i][0] << endl;
	}
	out.close();

	cout << "Cleaning up." << endl;
	fann_destroy_train(train_data);
	fann_destroy_train(test_data);
	fann_destroy(ann);
	
	return;
}
