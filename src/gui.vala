using Gtk;

namespace GMCPIL
{
	public weak string REPO_URL = "https://github.com/MCPI-Revival/gMCPIL";
	public weak string COPYRIGHT = "Copyright 2021 MCPI-Revival contributors";
	public weak string DESCRIPTION = "Simple launcher for MCPI-Reborn - Vala/GTK+ Edition.";
	public weak string DEFAULT_SERVERS = "thebrokenrail.com\npbptanarchy.tk\n";
	public weak string SERVERS_LABEL = "Each line is an ip:port pair. If the port is omitted, 19132 is used.";
	public weak string VERSION = "v0.13.0";

	public delegate void TabDelegate(Config config, Box tab) throws Error;

	public class Gui : Window
	{
		Box hbox;
		Box vbox;
		Box switcher_box;
		Stack stack;
		StackSwitcher switcher;
		HeaderBar header;
		Button launch_button;
		Button about_button;
		Config config;

		public Gui(Config config)
		{
			Object(
				title: "gMCPIL",
				default_width: -1,
				default_width: 360,
				icon_name: "gmcpil"
			);

			this.config = config;

			vbox = new Box(Orientation.VERTICAL, 0);

			hbox = new Box(Orientation.HORIZONTAL, 2);
			launch_button = new Button.with_label("Launch");
			launch_button.set_margin_end(2);
			launch_button.set_margin_bottom(2);

			stack = new Stack();
			switcher_box = new Box(Orientation.HORIZONTAL, 0);
			switcher = new StackSwitcher();
			switcher.set_stack(stack);

			about_button = new Button.from_icon_name("help-about-symbolic", IconSize.LARGE_TOOLBAR);

			header = new HeaderBar();
			header.set_show_close_button(true);
			header.set_custom_title(switcher_box);

			switcher_box.pack_start(switcher, true, false, 0);
			switcher_box.pack_end(about_button, true, false, 2);

			hbox.pack_end(launch_button, false, false, 0);

			vbox.pack_start(stack, true, true, 0);
			vbox.pack_start(hbox, false, false, 0);

			about_button.clicked.connect(about_cb);
			launch_button.clicked.connect(launch_cb);
			destroy.connect(() => {
				try
				{
					config.save();
				} catch (Error err)
				{
					stderr.printf("Error: %s.\n", err.message);
				}
				Gtk.main_quit();
			});

			set_titlebar(header);
			header.set_decoration_layout(":close");
			add(vbox);
		}

		void about_cb()
		{
			AboutDialog about_dialog;

			about_dialog = new AboutDialog();
			about_dialog.set_program_name("gMCPIL");
			about_dialog.set_logo_icon_name("gmcpil");
			about_dialog.set_version(VERSION);
			about_dialog.set_website(REPO_URL);
			about_dialog.set_copyright(COPYRIGHT);
			about_dialog.set_comments(DESCRIPTION);
			about_dialog.run();
			about_dialog.destroy();
			return;
		}

		void launch_cb()
		{
			Pid pid;
			string[] argv = {"minecraft-pi-reborn-client"};

			config.set_env();

			try
			{
				Process.spawn_async(
					null, argv, null,
					SpawnFlags.SEARCH_PATH | SpawnFlags.DO_NOT_REAP_CHILD,
					null, out pid
				);
			} catch (Error err)
			{
				stderr.printf("Error: %s.\n", err.message);
				return;
			}

			if (this.config.hide)
			{
				this.hide();
			}

			ChildWatch.add(pid, (pid, status) => {
				if (this.config.hide)
				{
					this.show_all();
				}
				Process.close_pid(pid);
			});
		}

		public void add_tab(string name, TabDelegate code) throws Error
		{
			Box tab;
			tab = new Box(Orientation.VERTICAL, 0);
			code(config, tab);
			stack.add_titled(tab, name, name);
			return;
		}
	}
}

public static int main(string[] args)
{
	string home;
	string path;
	GMCPIL.Config config;
	GMCPIL.Gui gui;

	Environment.set_variable("GTK_THEME", "Adwaita:dark", true);
	Gtk.init(ref args);
	try {
		home = Environment.get_variable("HOME");
		path = Path.build_filename(home, ".gmcpil.json");

		GMCPIL.Profile.init("/opt/minecraft-pi-reborn-client/available-feature-flags");
		config = GMCPIL.Config.from_file(path);
		gui = new GMCPIL.Gui(config);
		gui.add_tab("Play", GMCPIL.play_tab);
		gui.add_tab("Features", GMCPIL.features_tab);
		gui.add_tab("Servers", GMCPIL.servers_tab);
		gui.show_all();
		Gtk.main();
	} catch (Error err)
	{
		stderr.printf("Error: %s.\n", err.message);
		return -1;
	}
	return 0;
}
