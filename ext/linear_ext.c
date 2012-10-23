#include "ruby.h"
#include "/usr/include/linear.h"
#include <math.h>

#define MALLOC(type,n) (type *)malloc((n)*sizeof(type))
#define INF HUGE_VAL

VALUE cClassifier;
VALUE mLinearScience;
VALUE cModel;

static struct parameter * 
alloc_parameter(struct parameter * parameter_) {
	parameter_ = MALLOC(struct parameter, 1);
	return parameter_;
}

// static void 
// free_parameter(struct parameter * parameter_) {
// 	free(parameter_->weight_label);
// 	free(parameter_->weight);
// }

static struct problem * 
alloc_problem(struct problem * problem_) {
	problem_ = MALLOC(struct problem, 1);
	return problem_;
}

static void 
destroy_problem(struct problem * problem_) {
	int i;

	free(problem_->y);
	for(i = 0; i < problem_->l; i++) {
		free(problem_->x[i]);
	}
	free(problem_->x);
}

// static void
// free_model(struct model * model_) {

// }

static void 
set_param_weights(struct parameter * param, VALUE rb_weights) {
	VALUE rb_weight_labels = rb_funcall(rb_weights, rb_intern("keys"), 0);
	VALUE rb_weight_values = rb_funcall(rb_weights, rb_intern("vaules"), 0);
	long i;

	param->nr_weight = RHASH_SIZE(rb_weights);
	param->weight_label = MALLOC(int, param->nr_weight);
	param->weight = MALLOC(double, param->nr_weight);

	if(param->weight_label == NULL || param->weight == NULL){
		return;
	}

	for(i = 0; i < param->nr_weight; i++) {
		param->weight_label[i] = NUM2INT(rb_ary_entry(rb_weight_labels, i));
		param->weight[i] = NUM2DBL(rb_ary_entry(rb_weight_values, i));
	}
}

static void //FIXME
read_rb_examples(struct problem * c_problem, VALUE rb_example_ary, double c_bias) {
	int i, j;
	int current_length;
	int len_with_bias;
	VALUE rb_example_labels;
	VALUE rb_example_values;
	VALUE rb_current_example;

	for(i = 0; i < c_problem->l; i++) {
		rb_current_example = rb_ary_entry(rb_example_ary, i);
		current_length = RHASH_SIZE(rb_ary_entry(rb_current_example, 1));
		len_with_bias = c_bias > 0.0 ? current_length + 2 : current_length + 1;
		rb_example_labels = rb_funcall(rb_ary_entry(rb_current_example, 1), rb_intern("keys"), 0);
		rb_example_values = rb_funcall(rb_ary_entry(rb_current_example, 1), rb_intern("values"), 0);

		c_problem->x[i] = MALLOC(struct feature_node, len_with_bias);
		c_problem->y[i] = NUM2INT(rb_ary_entry(rb_current_example, 0));

		for(j = 0; j < current_length; j++) {
			c_problem->x[i][j].index = NUM2INT(rb_ary_entry(rb_example_labels, j));
			c_problem->x[i][j].value = NUM2DBL(rb_ary_entry(rb_example_values,j));
		}

		if(c_bias > 0) {
			c_problem->x[i][len_with_bias - 2].index = c_problem->n;
			c_problem->x[i][len_with_bias - 2].value = c_bias;
		}
		c_problem->x[i][len_with_bias - 1].index = -1;
	}
}

static void
read_problem(struct problem * c_problem, VALUE rb_problem, double c_bias) {
	VALUE rb_example_ary = rb_iv_get(rb_problem, "@examples");
	int max_dimension = NUM2INT(rb_funcall(rb_problem, rb_intern("max_dimension"),0));
	int i;

	c_problem->l = RARRAY_LEN(rb_example_ary); // l -> number of examples
	c_problem->n = c_bias > 0 ? max_dimension + 1 : max_dimension; 
	// n -> maximum dimension of feature space
	c_problem->bias = c_bias;
	c_problem->x = MALLOC(struct feature_node *, c_problem->l);
	c_problem->y = MALLOC(int, c_problem->l);

	read_rb_examples(c_problem, rb_example_ary, c_bias);
}

static void 
assign_param_value(struct parameter * param, VALUE rb_param_key, VALUE rb_param_value) {
	if(!NIL_P(rb_param_value)) {
		if(StringValueCStr(rb_param_key) == "kernel")
			param->solver_type = NUM2INT(rb_param_value);
		else if(StringValueCStr(rb_param_key) == "c_cost")
			param->C = NUM2DBL(rb_param_value);
		else if(StringValueCStr(rb_param_key) == "e_epsilon")
			param->eps = NUM2DBL(rb_param_value);
		else if(StringValueCStr(rb_param_key) == "weights")
			set_param_weights(param, rb_param_value);
	}
}

static void
read_parameters(struct parameter * param, VALUE rb_param_hash) {
	unsigned int i;
	VALUE rb_param_keys = rb_funcall(rb_param_hash, rb_intern("keys"), 0);
	VALUE rb_param_values = rb_funcall(rb_param_hash, rb_intern("values"), 0);

	for(i = 0; i < RHASH_SIZE(rb_param_hash); i++)
		assign_param_value(param, rb_ary_entry(rb_param_keys, i), rb_ary_entry(rb_param_values, i));
}

static void 
set_default_parameters(struct parameter * param) {
	param->solver_type = 1;
	param->C = 1;
	param->eps = INF;
	param->nr_weight = 0;
	param->weight_label = NULL;
	param->weight = NULL;
}

static VALUE
weights_to_hash(struct parameter param_) {
	VALUE rb_weights_hash = rb_hash_new();
	int i;

	for(i = 0; i < param_.nr_weight; i++) 
		rb_hash_aset(rb_weights_hash, INT2NUM(param_.weight_label[i]), rb_float_new(param_.weight[i]));

	return rb_weights_hash;
}

static VALUE 
params_to_hash(struct parameter param) {
	VALUE rb_param_hash = rb_hash_new();

	rb_hash_aset(rb_param_hash, rb_str_new2("kernel"),    INT2NUM(param.solver_type));
	rb_hash_aset(rb_param_hash, rb_str_new2("c_cost"),    rb_float_new(param.C));
	rb_hash_aset(rb_param_hash, rb_str_new2("e_epsilon"), rb_float_new(param.eps));
	rb_hash_aset(rb_param_hash, rb_str_new2("weights"),   weights_to_hash(param));

	return rb_param_hash;
}

static VALUE
c_data_to_model(VALUE self, struct model * model_) {
	int i;
	int w_size;

	rb_iv_set(self, "@parameters", params_to_hash(model_->param));
	rb_iv_set(self, "@nr_class", INT2NUM(model_->nr_class));
	rb_iv_set(self, "@nr_feature", INT2NUM(model_->nr_feature));
	rb_iv_set(self, "@bias", rb_float_new(model_->bias));

	VALUE rb_label = rb_ary_new();
	VALUE rb_w = rb_ary_new();

	w_size = model_->bias > 0 ? (model_->nr_class + 1) * model_->nr_feature : model_->nr_class * model_->nr_feature;

	for(i = 0; i < model_->nr_class; i++)
		rb_ary_push(rb_label, INT2NUM(model_->label[i]));

	for(i = 0; i < w_size; i++)
		rb_ary_push(rb_w, rb_float_new(model_->w[i]));

	rb_iv_set(self, "@label", rb_label);
	rb_iv_set(self, "@w", rb_w);

	return self;
}

static VALUE
c_classifier_train(VALUE self) {
	VALUE rb_param = rb_funcall(rb_iv_get(self, "@parameters"), rb_intern("pack"), 0);
	VALUE rb_problem = rb_iv_get(self, "@problem");
	VALUE rb_bias = rb_hash_aref(rb_param, rb_str_new2("bias"));

	struct model* c_model;
	struct parameter c_param;
	struct parameter param;
	struct problem c_problem;
	double c_bias = NIL_P(rb_bias) ? -1.0 : NUM2DBL(rb_bias);
	const char* error_msg;

	if(rb_problem == Qnil || rb_funcall(rb_iv_get(rb_problem, "@examples"), rb_intern("empty?"), 0) == Qtrue)
		rb_raise(rb_eRuntimeError, "No training examples provided.");

	set_default_parameters(&c_param);
	read_parameters(&c_param, rb_param);
	read_problem(&c_problem, rb_problem, c_bias);
	error_msg = check_parameter(&c_problem, &c_param);

	if(error_msg)
		rb_raise(rb_eRuntimeError, error_msg);
	
	c_model = train(&c_problem, &c_param);
	rb_iv_set(self, "@model", c_data_to_model(rb_iv_get(self, "@model"), c_model));

	free_and_destroy_model(&c_model);
	destroy_param(&c_param);
	destroy_problem(&c_problem);

	return rb_iv_get(self, "@model");
}

static VALUE
m_init(VALUE self) {
	rb_iv_set(self, "@parameters", rb_hash_new());
	rb_iv_set(self, "@nr_class", Qnil);
	rb_iv_set(self, "@nr_feature", Qnil);
	rb_iv_set(self, "@bias", Qnil);
	rb_iv_set(self, "@label", rb_ary_new());
	rb_iv_set(self, "@w", rb_ary_new());

	return self;
}

void Init_linear_ext() {
	mLinearScience = rb_define_module("LinearScience");

	cModel = rb_define_class_under(mLinearScience, "Model", rb_cObject);
	rb_define_method(cModel, "initialize", m_init, 0);


	cClassifier = rb_define_class_under(mLinearScience, "Classifier", rb_cObject);
	rb_define_method(cClassifier, "train", c_classifier_train, 0);
}