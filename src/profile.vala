namespace GMCPIL
{
	public enum ProfileType
	{
		VANILLA,
		MODDED,
		MCPE,
		OPTIMIZED,
		CUSTOM
	}

	public class Profile
	{
		public static Array<string> features;
		static Array<Object> profiles;
		static string[] optimized_disable = {
			"Fancy Graphics",
			"Smooth Lighting",
			"Animated Water",
			"Disable \"gui_blocks\" Atlas"
		};

		public static void init(string path) throws Error
		{
			File file;
			bool enabled;
			string line;
			string[] tuple;
			FileInputStream file_stream;
			DataInputStream data_stream;

			features = new Array<string>();
			profiles = new Array<Object>();

			for (ProfileType i = 0; i < ProfileType.CUSTOM; i += 1)
			{
				profiles.append_val(new Object());
			}

			file = File.new_for_path(path);
			file_stream = file.read();
			data_stream = new DataInputStream(file_stream);
			line = data_stream.read_line();
			while (line != null)
			{
				tuple = line.split(" ", 2);
				enabled = tuple[0][0] == 'T';
				features.append_val(tuple[1]);
				for (ProfileType i = 0; i < ProfileType.CUSTOM; i += 1)
				{
					switch (i)
					{
						case ProfileType.VANILLA:
							continue;
						case ProfileType.MODDED:
							if (tuple[1] == "Touch GUI")
							{
								continue;
							}
						break;
						case ProfileType.OPTIMIZED:
							if (tuple[1] in optimized_disable)
							{
								continue;
							}
						break;
						default:
						break;
					}
					get_profile(i).set_data<bool>(tuple[1], enabled);
				}
				line = data_stream.read_line();
			}
			return;
		}

		public static inline Object get_profile(ProfileType profile)
		{
			return profiles.index(profile);
		}

		public static Json.Object serialize(Object obj)
		{
			string name;
			bool enabled;
			Json.Object tmp;

			tmp = new Json.Object();
			for (int i = 0; i < features.length; i++)
			{
				name = features.index(i);
				enabled = obj.get_data<bool>(name);
				tmp.set_boolean_member(name, enabled);
			}
			return tmp;
		}

		public static Object deserialize(Json.Object obj)
		{
			string name;
			bool enabled;
			Object tmp;

			tmp = new Object();
			for (int i = 0; i < features.length; i++)
			{
				name = features.index(i);
				if (obj.has_member(name))
				{
					enabled = obj.get_boolean_member(name);
				} else
				{
					enabled = get_profile(ProfileType.MCPE).get_data<bool>(name);
				}
				tmp.set_data<bool>(name, enabled);
			}
			return tmp;
		}

		public static void set_custom_profile(Object profile)
		{
			if (profiles.length < ProfileType.CUSTOM)
			{
				profiles.append_val(profile);
			}
			return;
		}
	}
}
