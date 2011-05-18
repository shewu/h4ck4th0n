package map;

import gui.EntityEditor;
import gui.GoalEditor;
import gui.ObjectEditor;
import gui.SpawnEditor;
import gui.WallEditor;

public enum MapObjectType {
	WALL {
		@Override
		public ObjectEditor getEditor() {
			return new WallEditor();
		}
	}, GOAL {
		@Override
		public ObjectEditor getEditor() {
			return new GoalEditor();
		}
	}, SPAWN {
		@Override
		public ObjectEditor getEditor() {
			return new SpawnEditor();
		}
	}, ENTITY {
		@Override
		public ObjectEditor getEditor() {
			return new EntityEditor();
		}
	};

	public static final MapObjectType[] VALUES = values();

	public abstract ObjectEditor getEditor();
}
