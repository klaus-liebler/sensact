using SensactEditor.Components;
using SensactEditor.Services;

// Editor fuer das XML-Hausmodell. Lokales Werkzeug ohne Anmeldung: lauscht nur auf localhost
// (s. Properties/launchSettings.json) und schreibt direkt in model/*.xml im Git-Arbeitsverzeichnis.
var builder = WebApplication.CreateBuilder(args);

builder.Services.AddRazorComponents()
    .AddInteractiveServerComponents();
// Ein Modell fuer alle offenen Tabs: Aenderungen in einem Tab erscheinen sofort in den anderen.
builder.Services.AddSingleton<HouseModelService>();

var app = builder.Build();

if (!app.Environment.IsDevelopment())
{
    app.UseExceptionHandler("/Error", createScopeForErrors: true);
}
app.UseStatusCodePagesWithReExecute("/not-found", createScopeForStatusCodePages: true);

app.MapStaticAssets();
app.MapRazorComponents<App>()
    .AddInteractiveServerRenderMode();

app.Run();
