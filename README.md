# netcf-ruby

Ruby bindings for [NetCF](https://fedorahosted.org/netcf/).


## Usage

```ruby
require 'netcf'

n = Netcf.new
p n.list_interfaces(NetcfIf::ACTIVE)
i = n.lookup_by_name('lo')
p i.status == NetcfIf::ACTIVE
p i.xml_desc
p i.xml_state
```
