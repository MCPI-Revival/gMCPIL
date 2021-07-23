using Gtk;

namespace GMCPIL
{
	public void servers_tab(Config config, Box tab) throws Error
	{
		int64 sz;
		uint8[]? buff;
		string home;
		string filename;
		string dirname;
		File file;
		File dir;
		FileInfo info;
		DataInputStream stream;
		Label label;
		TextView view;
		ScrolledWindow scroll;

		home = Environment.get_variable("HOME");
		dirname = Path.build_filename(home, ".minecraft-pi");
		filename = Path.build_filename(dirname, "servers.txt");

		label = new Label(SERVERS_LABEL);
		label.set_line_wrap(true);
		label.set_justify(Justification.CENTER);
		label.set_margin_top(6);

		scroll = new ScrolledWindow(null, null);
		scroll.set_policy(PolicyType.AUTOMATIC, PolicyType.AUTOMATIC);
		scroll.set_shadow_type(ShadowType.NONE);
		scroll.set_border_width(5);

		dir = File.new_for_path(dirname);
		if (!dir.query_exists())
		{
			dir.make_directory(null);
		}

		file = File.new_for_path(filename);
		if (!file.query_exists())
		{
			file.create(FileCreateFlags.NONE);
		}
		stream = new DataInputStream(file.read());
		info = file.query_info("*", FileQueryInfoFlags.NONE);
		sz = info.get_size();
		buff = null;

		config.servers = new TextBuffer(null);
		if (sz > 0)
		{
			buff = new uint8[sz + 1];
			stream.read(buff);
			buff[sz] = 0x00;
			config.servers.set_text((string)buff);
		} else
		{
			config.servers.set_text(DEFAULT_SERVERS);
		}

		view = new TextView.with_buffer(config.servers);
		view.set_editable(true);
		view.set_cursor_visible(true);

		scroll.add(view);
		tab.pack_start(label, false, false, 0);
		tab.pack_start(scroll, true, true, 0);
	}
}
