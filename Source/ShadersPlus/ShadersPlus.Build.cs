using UnrealBuildTool;

public class ShadersPlus : ModuleRules
{
	public ShadersPlus(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        if (Target.Version.MinorVersion <= 19)
        {
            PublicIncludePaths.AddRange(
			    new string[] {
				    "Public"
			    });

		    PrivateIncludePaths.AddRange(
			    new string[] {
				    "Private"
			    });
        }

		PublicDependencyModuleNames.AddRange(
			new string[] {
				"Core",
			    "RenderCore",
			    "ShaderCore",
			    "RHI"
            });

		PrivateDependencyModuleNames.AddRange(
			new string[] {
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore"
			});
	}
}
