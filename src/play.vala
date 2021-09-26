using Gtk;

namespace GMCPIL
{
	class SettingBox : Box
	{
		public SettingBox(string name, Widget widget)
		{
			Object(orientation: Orientation.HORIZONTAL, spacing: 0);
			pack_start(new Label(name), false, false, 10);
			pack_end(widget, false, false, 10);
		}
	}

	public string get_splash_text()
	{
		string[] splashes = {
			"Sexy!",
			"We Fixed The Rail!",
			"Who Broke The Rail?",
			"The World Of Notch!",
			"WDYM Discontinued?",
			"Oh, ok, Pigmen...",
			"Check out the non-existant Far Lands!",
			"mcpirevival.tk!",
			"#alvarito-come-back",
			"Classic!",
			"Wow! Modded MCPI!",
			"Now with fly-hacks!",
			"Where Is My Pie?",
			"I was promised pie!",
			"@Banana",
			"BANANA!",
			"To \"Na\" Or Not To \"Na\", that is the question!",
			"Not Minecraft Java!",
			"Definitely not Minecraft Console!",
			"Oh yeah, that version...",
			"Join The Discord!",
			"Segmentation fault (core dumped)",
			"It's the Segment's Fault!",
			"As seen on Discord!",
			"Obfuscated!",
			"As not seen on TV!",
			"Why must you hurt me in this way?",
			"Who is StevePi?",
			"(Not) Made by Notch!",
			"Open-Source!",
			"FREE!",
			"RIP MCPIL-Legacy, 2020-2020",
			"StevePi = popbob confirmed?",
			"Watch out for StevePi!",
			"Didn't remove Herobrine!",
			"Minceraft",
			"MCPI-Docker",
			"MCPI-Devs",
			"RIP Picraft",
			"Check out PBPT Anarchy!",
			"Now without Docker!",
			"Banana!",
			"Fancy Vala",
			"Awesome Community!",
			"Awesome!",
			"Minecraft!",
			"Now in 3D!",
			"Pretty!",
			"Singleplayer!",
			"Survive!",
			"Try it!",
			"你好中国",
			"한국 안녕하세요!",
			"日本ハロー！",
			"Привет Россия!",
			"Γεια σου Ελλάδα!",
			"Water proof!",
			"Supercalifragilisticexpialidocious!",
			"Pixels!",
			"Now With Multiplayer!",
			"Keyboard compatible!",
			"Exploding creepers!",
			"Enhanced!",
			"Don't look directly at the bugs!",
			"Fantasy!",
			"Create!",
			"Casual gaming!"
		};
		return splashes[Random.int_range(0, splashes.length - 1)];
	}

	public void play_tab(Config config, Box tab) throws Error
	{
		Box hide_hbox;
		Box title_hbox;
		Label title;
		Label splash;
		Label description;
		Entry hud;
		Entry username;
		ComboBoxText profile;
		ComboBoxText distance;
		CheckButton hide;
		string splash_text;
		string[] profile_names = {
			"Classic MCPI",
			"Modded MCPI",
			"Modded MCPE",
			"Optimized MCPE",
			"Custom Profile"
		};
		string[] profile_descriptions = {
			"Classic Minecraft Pi Edition (Not Recommended):\nAll optional features disabled.",
			"Modded Minecraft Pi Edition:\nDefault MCPI-Reborn optional features without Touch GUI.",
			"Modded Minecraft Pocket Edition (Recommended):\nDefault MCPI-Reborn optional features.",
			"Optimized Minecraft Pocket Edition:\nDefault MCPI-Reborn features with lower quality graphics.",
			"Custom Profile: Modify its settings in the Features tab."
		};

		splash_text = @"<span foreground=\"#ffff00\" size=\"12000\">$(get_splash_text())</span>\n";

		title_hbox = new Box(Orientation.HORIZONTAL, 0);

		title = new Label(null);
		title.set_markup("<span size=\"24000\">Minecraft Pi Launcher</span>");

		splash = new Label(null);
		splash.set_markup(splash_text);

		username = new Entry();
		username.set_text(config.username);
		username.set_size_request(25 * 10, -1);

		hud = new Entry();
		hud.set_text(config.hud);
		hud.set_size_request(25 * 10, -1);

		distance = new ComboBoxText();
		for (Distance i = 0; i <= Distance.FAR; i += 1)
		{
			distance.append_text(Config.distance_names[i]);
		}
		distance.set_active(config.distance);
		distance.set_size_request(25 * 10, -1);

		profile = new ComboBoxText();
		for (ProfileType i = 0; i <= ProfileType.CUSTOM; i += 1)
		{
			profile.append_text(profile_names[i]);
		}
		profile.set_active(config.profile);
		profile.set_size_request(25 * 10, -1);

		hide_hbox = new Box(Orientation.HORIZONTAL, 0);
		hide = new CheckButton.with_label("Hide launcher");
		hide.set_active(config.hide);
		hide.set_size_request(-1, 32); /* To match other input widgets */

		description = new Label(profile_descriptions[config.profile]);
		description.set_justify(Justification.CENTER);
		description.set_line_wrap(true);

		title_hbox.pack_start(title, true, false, 0);

		tab.pack_start(title_hbox, false, false, 4);
		tab.pack_start(splash, false, false, 0);

		hide_hbox.pack_end(hide, false, false, 10);

		tab.pack_start(new SettingBox("Username", username), false, false, 2);
		tab.pack_start(new SettingBox("Gallium HUD options", hud), false, false, 2);
		tab.pack_start(new SettingBox("Distance", distance), false, false, 2);
		tab.pack_start(new SettingBox("Profile", profile), false, false, 2);
		tab.pack_start(hide_hbox, false, false, 2);
		tab.pack_start(description, false, false, 2);

		username.changed.connect(() => {
			config.username = username.get_text();
		});
		hud.changed.connect(() => {
			config.hud = hud.get_text();
		});
		distance.changed.connect(() => {
			config.distance = (Distance)distance.get_active();
		});
		profile.changed.connect(() => {
			config.profile = (ProfileType)profile.get_active();
			description.set_text(profile_descriptions[config.profile]);
		});
		hide.toggled.connect(() => {
			config.hide = hide.get_active();
		});
	}
}
