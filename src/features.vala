using Gtk;

namespace GMCPIL
{
	public class FeatureButton : Bin
	{
		private string feat_name;
		private Config config;
		private CheckButton button;

		public FeatureButton(Config config, string feat_name, bool feat_enabled)
		{
			Object();
			this.feat_name = feat_name;
			this.config = config;

			button = new CheckButton.with_label(feat_name);
			button.set_active(feat_enabled);
			button.toggled.connect(toggled_cb);
			add_with_properties(button, null);
		}

		private void toggled_cb()
		{
			config.features.set_data<bool>(feat_name, button.get_active());
			config.modified = true;
		}
	}

	public void features_tab(Config config, Box tab) throws Error
	{
		string name;
		bool enabled;
		Box list_vbox;
		FeatureButton button;
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
			button = new FeatureButton(config, name, enabled);
			list_vbox.pack_start(button, false, false, 0);
		}

		scroll.add(list_vbox);
		tab.pack_start(scroll, true, true, 4);
	}
}
