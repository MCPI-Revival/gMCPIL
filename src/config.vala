using Gtk;

namespace GMCPIL
{
	public enum Distance
	{
		TINY,
		SHORT,
		NORMAL,
		FAR
	}

	public class Config : Object, Json.Serializable
	{
		public bool hide { get; set; default = true; }
		public string hud { get; set; default = "simple,fps"; }
		public string username { get; set; default = "StevePi"; }
		public Object features { get; set; }
		public ProfileType profile { get; set; }
		public Distance distance { get; set; default = Distance.SHORT; }
		public bool modified = false;
		string filename;
		public TextBuffer servers;

		public static string[] distance_names = {
			"Tiny",
			"Short",
			"Normal",
			"Far"
		};

		construct
		{
			this.notify.connect(() => {
				modified = true;
			});
		}

		public static Config from_file(string filename) throws Error
		{
			File file;
			FileInputStream stream;
			Json.Parser parser;
			Json.Node root;
			Json.Object root_obj;
			GMCPIL.Config config;

			file = File.new_for_path(filename);
			if (!file.query_exists())
			{
				file.create(FileCreateFlags.NONE);
			}
			stream = file.read();

			parser = new Json.Parser();
			parser.load_from_stream(stream);

			root = parser.get_root();
			if (root == null)
			{
				root = new Json.Node(Json.NodeType.OBJECT);
				root_obj = new Json.Object();
				root.set_object(root_obj);
			}

			config = Json.gobject_deserialize(typeof(GMCPIL.Config), root) as GMCPIL.Config;
			config.filename = filename;
			if (config.features == null)
			{
				config.features = Profile.deserialize(new Json.Object());
			}
			stream.close();
			return config;
		}

		public void save() throws Error
		{
			File file;
			string home;
			string buff;
			string servers_path;
			TextIter start;
			TextIter end;
			DataOutputStream stream;
			Json.Node root;
			Json.Generator gen;

			root = Json.gobject_serialize(this);
			gen = new Json.Generator();
			gen.set_root(root);
			gen.to_file(filename);

			servers.get_bounds(out start, out end);
			buff = servers.get_text(start, end, false);

			home = Environment.get_variable("HOME");
			servers_path = Path.build_filename(home, ".minecraft-pi", "servers.txt");
			file = File.new_for_path(servers_path);
			stream = new DataOutputStream(file.replace(null, false, FileCreateFlags.NONE));
			stream.put_string(buff, null);
			return;
		}

		public void set_env()
		{
			string name;
			bool enabled;
			StringBuilder builder;

			if (!modified)
			{
				return;
			}
			builder = new StringBuilder();
			for (int i = 0; i < Profile.features.length; i++)
			{
				name = Profile.features.index(i);
				enabled = features.get_data<bool>(name);
				if (enabled)
				{
					builder.append_printf("%s|", name);
				}
			}
			Environment.set_variable("GALLIUM_HUD", hud, true);
			Environment.set_variable("MCPI_USERNAME", username, true);
			Environment.set_variable("MCPI_FEATURE_FLAGS", builder.str, true);
			Environment.set_variable("MCPI_RENDER_DISTANCE", distance_names[distance], true);
			modified = false;
			return;
		}

		public Json.Node serialize_property(string name, Value val, ParamSpec spec)
		{
			Json.Node node;

			if (name != "features")
			{
				return default_serialize_property(name, val, spec);
			}

			node = new Json.Node(Json.NodeType.OBJECT);
			node.set_object(Profile.serialize(val.get_object()));
			return node;
		}

		public bool deserialize_property(string name, out Value val, ParamSpec spec, Json.Node node)
		{
			if (name != "features")
			{
				return default_deserialize_property(name, out val, spec, node);
			}

			val = Value(typeof(Object));
			val.set_object(Profile.deserialize(node.get_object()));
			return true;
		}
	}
}
