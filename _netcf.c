#include <netcf.h>
#include <ruby.h>


VALUE cNetcf;
VALUE cNetcfIf;
VALUE eNetcfError;


static void
_raise_NetcfError_netcf(struct netcf *_netcf)
{
	const char *_errmsg;
	const char *_details;
	int _code = ncf_error(_netcf, &_errmsg, &_details);
	const char *_fmt;
	if (_details == NULL) {
		_fmt = "%s (%d) (%s)";
	} else {
		_fmt = "%s (%d)";
	}
	rb_raise(eNetcfError, _fmt, _errmsg, _code, _details);
}


static VALUE
Netcf_new(int argc, VALUE *argv, VALUE class)
{
	VALUE root;
	rb_scan_args(argc, argv, "01", &root);
	struct netcf *_netcf;
	char *_root = root == Qnil ? NULL : StringValuePtr(root);
	int _result = ncf_init(&_netcf, _root);
	if (_result != 0) {
		rb_raise(eNetcfError, "ncf_init returned %d", _result);
	}
	VALUE netcf = Data_Wrap_Struct(class, NULL, ncf_close, _netcf);
	rb_obj_call_init(netcf, 0, NULL);
	return netcf;
}


static VALUE
Netcf_change_begin(VALUE self, VALUE flags)
{
	struct netcf *_netcf;
	Data_Get_Struct(self, struct netcf, _netcf);
	return INT2FIX(ncf_change_begin(_netcf, NUM2UINT(flags)));
}


static VALUE
Netcf_change_rollback(VALUE self, VALUE flags)
{
	struct netcf *_netcf;
	Data_Get_Struct(self, struct netcf, _netcf);
	return INT2FIX(ncf_change_rollback(_netcf, NUM2UINT(flags)));
}


static VALUE
Netcf_change_commit(VALUE self, VALUE flags)
{
	struct netcf *_netcf;
	Data_Get_Struct(self, struct netcf, _netcf);
	return INT2FIX(ncf_change_commit(_netcf, NUM2UINT(flags)));
}


static VALUE
Netcf_error(VALUE self)
{
	struct netcf *_netcf;
	Data_Get_Struct(self, struct netcf, _netcf);
	const char *_errmsg;
	const char *_details;
	int _result = ncf_error(_netcf, &_errmsg, &_details);
	VALUE errmsg = _errmsg == NULL ? Qnil : rb_str_new2(_errmsg);
	VALUE details = _details == NULL ? Qnil : rb_str_new2(_details);
	return rb_ary_new3(3, INT2FIX(_result), errmsg, details);
}


static VALUE
Netcf_define(VALUE self, VALUE xml)
{
	struct netcf *_netcf;
	Data_Get_Struct(self, struct netcf, _netcf);
	char *_xml = StringValuePtr(xml);
	struct netcf_if *_netcf_if = ncf_define(_netcf, _xml);
	if (_netcf_if == NULL) {
		_raise_NetcfError_netcf(_netcf);
	}
	VALUE netcf_if = Data_Wrap_Struct(cNetcfIf, NULL, ncf_if_free, _netcf_if);
	rb_obj_call_init(netcf_if, 0, NULL);
	return netcf_if;
}


static VALUE
Netcf_list_interfaces(VALUE self, VALUE flags)
{
	struct netcf *_netcf;
	Data_Get_Struct(self, struct netcf, _netcf);
	unsigned int _flags = NUM2UINT(flags);
	int _maxnames = ncf_num_of_interfaces(_netcf, _flags);
	char **_names = ALLOC_N(char *, _maxnames);
	int _result = ncf_list_interfaces(_netcf, _maxnames, _names, _flags);
	if (_result < 0) {
		_raise_NetcfError_netcf(_netcf);
	}
	VALUE names = rb_ary_new2(_result);
	int i;
	for (i = 0; i < _result; ++i) {
		rb_ary_store(names, i, rb_str_new2(_names[i]));
	}
	return names;
}


static VALUE
Netcf_lookup_by_name(VALUE self, VALUE name)
{
	struct netcf *_netcf;
	Data_Get_Struct(self, struct netcf, _netcf);
	char *_name = StringValuePtr(name);
	struct netcf_if *_netcf_if = ncf_lookup_by_name(_netcf, _name);
	if (_netcf_if == NULL) {
		return Qnil;
	}
	VALUE netcf_if = Data_Wrap_Struct(cNetcfIf, NULL, ncf_if_free, _netcf_if);
	rb_obj_call_init(netcf_if, 0, NULL);
	return netcf_if;
}


static VALUE
Netcf_num_of_interfaces(VALUE self, VALUE flags)
{
	struct netcf *_netcf;
	Data_Get_Struct(self, struct netcf, _netcf);
	int _result = ncf_num_of_interfaces(_netcf, NUM2UINT(flags));
	if (_result < 0) {
		rb_raise(eNetcfError, "ncf_num_of_interfaces returned %d", _result);
	}
	return INT2FIX(_result);
}


static VALUE
NetcfIf_down(VALUE self)
{
	struct netcf_if *_netcf_if;
	Data_Get_Struct(self, struct netcf_if, _netcf_if);
	int _result = ncf_if_down(_netcf_if);
	if (_result != 0) {
		rb_raise(eNetcfError, "ncf_if_down returned %d", _result);
	}
	return Qnil;
}


static VALUE
NetcfIf_mac_string(VALUE self)
{
	struct netcf_if *_netcf_if;
	Data_Get_Struct(self, struct netcf_if, _netcf_if);
	const char *_mac_string = ncf_if_mac_string(_netcf_if);
	return _mac_string == NULL ? Qnil : rb_str_new2(_mac_string);
}


static VALUE
NetcfIf_name(VALUE self)
{
	struct netcf_if *_netcf_if;
	Data_Get_Struct(self, struct netcf_if, _netcf_if);
	const char *_name = ncf_if_name(_netcf_if);
	return _name == NULL ? Qnil : rb_str_new2(_name);
}


static VALUE
NetcfIf_status(VALUE self)
{
	struct netcf_if *_netcf_if;
	Data_Get_Struct(self, struct netcf_if, _netcf_if);
	unsigned int _flags;
	int _result = ncf_if_status(_netcf_if, &_flags);
	if (_result != 0) {
		rb_raise(eNetcfError, "ncf_if_status returned %d", _result);
	}
	return INT2FIX(_flags);
}


static VALUE
NetcfIf_undefine(VALUE self)
{
	struct netcf_if *_netcf_if;
	Data_Get_Struct(self, struct netcf_if, _netcf_if);
	int _result = ncf_if_undefine(_netcf_if);
	if (_result != 0) {
		rb_raise(eNetcfError, "ncf_if_undefine returned %d", _result);
	}
	return Qnil;
}


static VALUE
NetcfIf_up(VALUE self)
{
	struct netcf_if *_netcf_if;
	Data_Get_Struct(self, struct netcf_if, _netcf_if);
	int _result = ncf_if_up(_netcf_if);
	if (_result != 0) {
		rb_raise(eNetcfError, "ncf_if_up returned %d", _result);
	}
	return Qnil;
}


static VALUE
NetcfIf_xml_desc(VALUE self)
{
	struct netcf_if *_netcf_if;
	Data_Get_Struct(self, struct netcf_if, _netcf_if);
	const char *_xml_desc = ncf_if_xml_desc(_netcf_if);
	return _xml_desc == NULL ? Qnil : rb_str_new2(_xml_desc);
}


static VALUE
NetcfIf_xml_state(VALUE self)
{
	struct netcf_if *_netcf_if;
	Data_Get_Struct(self, struct netcf_if, _netcf_if);
	const char *_xml_state = ncf_if_xml_state(_netcf_if);
	return _xml_state == NULL ? Qnil : rb_str_new2(_xml_state);
}


void
Init__netcf() {

	cNetcf = rb_define_class("Netcf", rb_cObject);

	rb_define_const(cNetcf, "NOERROR", INT2FIX(NETCF_NOERROR));
	rb_define_const(cNetcf, "EINTERNAL", INT2FIX(NETCF_EINTERNAL));
	rb_define_const(cNetcf, "EOTHER", INT2FIX(NETCF_EOTHER));
	rb_define_const(cNetcf, "ENOMEM", INT2FIX(NETCF_ENOMEM));
	rb_define_const(cNetcf, "EXMLPARSER", INT2FIX(NETCF_EXMLPARSER));
	rb_define_const(cNetcf, "EXMLINVALID", INT2FIX(NETCF_EXMLINVALID));
	rb_define_const(cNetcf, "ENOENT", INT2FIX(NETCF_ENOENT));
	rb_define_const(cNetcf, "EEXEC", INT2FIX(NETCF_EEXEC));
	rb_define_const(cNetcf, "EINUSE", INT2FIX(NETCF_EINUSE));
	rb_define_const(cNetcf, "EXSLTFAILED", INT2FIX(NETCF_EXSLTFAILED));
	rb_define_const(cNetcf, "EFILE", INT2FIX(NETCF_EFILE));
	rb_define_const(cNetcf, "EIOCTL", INT2FIX(NETCF_EIOCTL));
	rb_define_const(cNetcf, "ENETLINK", INT2FIX(NETCF_ENETLINK));
	rb_define_const(cNetcf, "EINVALIDOP", INT2FIX(NETCF_EINVALIDOP));

	rb_define_singleton_method(cNetcf, "new", Netcf_new, -1);
	rb_define_method(cNetcf, "change_begin", Netcf_change_begin, 1);
	rb_define_method(cNetcf, "change_commit", Netcf_change_commit, 1);
	rb_define_method(cNetcf, "change_rollback", Netcf_change_rollback, 1);
	rb_define_method(cNetcf, "define", Netcf_define, 1);
	rb_define_method(cNetcf, "error", Netcf_error, 0);
	rb_define_method(cNetcf, "list_interfaces", Netcf_list_interfaces, 1);
	rb_define_method(cNetcf, "lookup_by_name", Netcf_lookup_by_name, 1);
	rb_define_method(cNetcf, "num_of_interfaces", Netcf_num_of_interfaces, 1);

	cNetcfIf = rb_define_class("NetcfIf", rb_cObject);

	rb_define_const(cNetcfIf, "INACTIVE", INT2FIX(NETCF_IFACE_INACTIVE));
	rb_define_const(cNetcfIf, "ACTIVE", INT2FIX(NETCF_IFACE_ACTIVE));

	rb_define_method(cNetcfIf, "down", NetcfIf_down, 0);
	rb_define_method(cNetcfIf, "mac_string", NetcfIf_mac_string, 0);
	rb_define_method(cNetcfIf, "name", NetcfIf_name, 0);
	rb_define_method(cNetcfIf, "status", NetcfIf_status, 0);
	rb_define_method(cNetcfIf, "undefine", NetcfIf_undefine, 0);
	rb_define_method(cNetcfIf, "up", NetcfIf_up, 0);
	rb_define_method(cNetcfIf, "xml_desc", NetcfIf_xml_desc, 0);
	rb_define_method(cNetcfIf, "xml_state", NetcfIf_xml_state, 0);

	eNetcfError = rb_define_class("NetcfException", rb_eRuntimeError);

}
