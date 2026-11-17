using SensactEditor.Components;
using SensactEditor.Services;

// Editor fuer das XML-Hausmodell. Lokales Werkzeug ohne Anmeldung: lauscht nur auf localhost
// (s. Properties/launchSettings.json) und schreibt direkt in model/*.xml im Git-Arbeitsverzeichnis.
var builder = WebApplication.CreateBuilder(args);

builder.Services.AddRazorComponents()
    .AddInteractiveServerComponents();
// Ein Modell fuer alle offenen Tabs: Aenderungen in einem Tab erscheinen sofort in den anderen.
builder.Services.AddSingleton<HouseModelService>();
builder.Services.AddSingleton<PackageBuildService>();

var app = builder.Build();

if (!app.Environment.IsDevelopment())
{
    app.UseExceptionHandler("/Error", createScopeForErrors: true);
}
app.UseStatusCodePagesWithReExecute("/not-found", createScopeForStatusCodePages: true);

app.MapStaticAssets();
// Fertiges Stationspaket herunterladen (deploy/<Station>/<Station>.pkg), s. PackageBuildService
app.MapGet("/download/{nodeId}", (string nodeId, PackageBuildService builds) =>
{
    if (!PackageBuildService.NodeIdPattern().IsMatch(nodeId)) return Results.BadRequest();
    var path = builds.PackagePath(nodeId);
    return File.Exists(path) ? Results.File(path, "application/octet-stream", nodeId + ".pkg") : Results.NotFound();
});
app.MapRazorComponents<App>()
    .AddInteractiveServerRenderMode();

app.Run();
