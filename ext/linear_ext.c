#include "liblinear-1.91/linear.h"
#include "ruby.h"

#define MALLOC(type,n) (type *)malloc((n)*sizeof(type))

VALUE cClassifier;
VALUE mLinearScience;
static int id_puts;

static VALUE rb_puts 
(VALUE obj) {
	return rb_funcall(rb_cObject, rb_intern("puts"), 1,  obj);
}

static VALUE 
each_block(VALUE yield_value, VALUE data2, int argc, VALUE argv[]) {
	rb_puts(yield_value);
	return Qtrue;	
}

static VALUE
rb_block_call_test(VALUE self, VALUE obj) {
	rb_puts(INT2NUM(RHASH_SIZE(obj)));
	rb_block_call(obj,
								rb_intern("each_key"),
								0,
								NULL,
								each_block,
								Qnil
								);
	return Qnil;
}

static struct parameter * alloc_parameter
(struct parameter * param) {
	param = MALLOC(struct parameter, 1);
	return param;
}

static void free_parameters
(struct parameter * param) {
	free(param->weight_label);
	free(param->weight);
}

static void set_param_weights
(struct parameter * param, VALUE rb_weigths) {
	VALUE rb_weight_labels = rb_funcall(rb_weights, rb_intern("keys"), 0);
	VALUE rb_weight_values = rb_funcall(rb_weights, rb_intern("vaules"),0);
	long i;

	param->nr_weight = RHASH_SIZE(rb_weights);
	param->weight_label = MALLOC(int, param->nr_weight);
	param->weight = MALLOC(double, param->nr_weight);

	if(param->weight_label == NULL || param->weight == NULL){
		return;
	}

	for(i = 0; i < param->nr_weight, i++) {
		param->weight_label[i] = rb_ary_entry(rb_weight_labels, i);
		param->weight[i] = rb_ary_entry(rb_weight_values, i);
	}
} 

static void assign_param_value 
(struct parameter * param, VALUE rb_param_key, VALUE rb_param_value) {
	switch(StringValueCStr(rb_param_key) {
		case "kernel":
			param->solver_type = FIX2INT(rb_param_value);
			break;
		case "c_cost":
			param->C = FIX2INT(rb_param_value);
			break;
		case "p_epsilon":
			param->p = FIX2INT(rb_param_value);
			break;
		case "e_epsilon":
			param->eps = FIX2INT(rb_param_value);
			break;
		case "weights"
			set_param_weights(param, rb_param_value);
			break;
		default:
			break;
	}
}


static struct parameter * read_parameters
(VALUE self, VALUE some_hash) {
	struct parameter * params;
	const char * param_keys[] = 
		{"kernel", "c_cost", "p_epsilon", "e_epsilon", "bias", "n_fold_cv", "weights"};
	int i;

	params = alloc_parameter(params);

	for (i = 0; i < 7; i++) {
		VALUE hash_value = rb_hash_aref(some_hash, rb_str_new2(c_params[i]));
		if (!NIL_P(hash_value)) {
			rb_funcall(rb_cObject, id_puts, 1, rb_str_new2(c_params[i]));
			rb_funcall(rb_cObject, id_puts, 1, hash_value);
		}
	}

	return params;
}

void Init_linear_ext() {
	mLinearScience = rb_define_module("LinearScience");
	cClassifier = rb_define_class_under(mLinearScience, "Classifier", rb_cObject);
	//rb_define_singleton_method(cClassifier, "read_parameters", c_read_parameters, 1);
	id_puts = rb_intern("puts");
	rb_define_global_function("rb_block_call_test", rb_block_call_test, 1);
}