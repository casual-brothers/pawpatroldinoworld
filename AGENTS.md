## Switch2 Build

- Cuando haya que compilar `Switch2` sin `cook/pak/package`, usar este comando:

```bat
C:\PDW\Engine\Build\BatchFiles\Build.bat PDW Switch2 Development -Project="C:\PDW\PDW\PDW.uproject" -remoteini="C:\PDW\PDW" -skipdeploy
```

## Switch2 Context

- SDK real usado para `Switch2/Ounce`:
  - `C:\Nintendo\NativeSDK20.5.28_Ounce\NintendoSDK`
- El plugin de engine para `StreamPlay` está en:
  - `C:\PDW\Engine\Platforms\Switch2\Plugins\Online\StreamPlay`
- `StreamPlay` ya está declarado en:
  - `C:\PDW\PDW\PDW.uproject`
- El proyecto usa esta config de plataforma:
  - `C:\PDW\PDW\Platforms\Switch2\Config\Switch2Engine.ini`

## Switch2 Notes

- `nn::streamplay` existe en el SDK real de `Switch2/Ounce` y parece ser la base SDK de `GameShare/StreamPlay`.
- El engine ya trae una capa Unreal en `FStreamPlay`, pero no parece tener expuesta la ruta avanzada de `Individual` para vídeo/audio por guest.
- El juego tiene integración base de `Guest`/`StreamPlay` en:
  - `C:\PDW\PDW\Source\PDW\Private\Managers\PDWStreamPlaySubsystem.cpp`
  - `C:\PDW\PDW\Source\PDW\Private\Gameplay\FSMFlow\States\PDWPauseFSMState.cpp`

## Known Build Blockers

- La compilación `Switch2` llega a link, pero ahora mismo falla por `FMODStudio` porque en este workspace no existen binarios/librerías de `FMOD` para `Switch2`.
- Ruta comprobada sin binarios `Switch2`:
  - `C:\PDW\PDW\Plugins\FMODStudio\Binaries`
- Error típico actual:
  - `ld.lld: error: unable to find library ... fmodL`
  - `ld.lld: error: unable to find library ... fmodstudioL`
