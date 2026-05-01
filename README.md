# gdlib

Reusable gameplay primitives for Godot 4. Builds as a custom C++ module today;
GDExtension build target planned.

## Primitives

- **state_machine** — `State` + `StateMachine` nodes. Animation-aware (concurrent
  enter/exit via separate `AnimationPlayer` references), data-driven transition
  rules with virtual override for dynamic logic, transition-rejected signal.

## Use

Add to your project's `gdpm.toml`:

```toml
[modules.gdlib]
url = "https://github.com/monokrome/gdlib"
commit = "<pin a commit>"
```

Then `gdpm install` and `gdpm build`.

## License

BSD 2-Clause. See `LICENSE.txt`.
