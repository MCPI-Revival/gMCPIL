using Gtk;

namespace GMCPIL
{
	public void features_tab(Config config, Box tab) throws Error
	{
		string name;
		bool enabled;
		Box list_vbox;
		CheckButton button;
		ScrolledWindow scroll;

		list_vbox = new Box(Orientation.VERTICAL, 0);
		scroll = new ScrolledWindow(null, null);
		scroll.set_policy(PolicyType.AUTOMATIC, PolicyType.AUTOMATIC);
		scroll.set_shadow_type(ShadowType.NONE);
		scroll.set_border_width(0);

		for (int i = 0; i < Profile.features.length; i++)
		{
			name = Profile.features.index(i);
			enabled = config.features.get_data<bool>(name);
			button = new CheckButton.with_label(name);
			button.set_active(enabled);
			button.toggled.connect(() => {
				config.features.set_data<bool>(name, button.get_active());
				config.modified = true;
			});
			list_vbox.pack_start(button, false, false, 0);
		}

		scroll.add(list_vbox);
		tab.pack_start(scroll, true, true, 4);
	}
}
