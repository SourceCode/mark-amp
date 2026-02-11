import React, { useRef, useState } from 'react';
import { Theme } from '../types';
import { X, Upload, Download, Check, AlertCircle } from 'lucide-react';

interface ThemeGalleryProps {
  isOpen: boolean;
  onClose: () => void;
  themes: Theme[];
  currentTheme: Theme;
  onApplyTheme: (theme: Theme) => void;
  onImportTheme: (theme: Theme) => void;
}

const ThemePreview: React.FC<{ theme: Theme; isActive: boolean; onClick: () => void; onExport: (e: React.MouseEvent) => void }> = ({ theme, isActive, onClick, onExport }) => {
  const colors = theme.colors;
  
  return (
    <div 
      className={`
        group relative cursor-pointer rounded-lg border-2 transition-all duration-200 overflow-hidden
        ${isActive ? 'border-win-accent ring-2 ring-win-accent/30 scale-105 z-10' : 'border-win-borderDark hover:border-win-borderLight'}
      `}
      onClick={onClick}
    >
      {/* Live Preview Simulation */}
      <div className="h-28 w-full flex flex-col" style={{ backgroundColor: colors['--bg-app'] }}>
        {/* Header Preview */}
        <div className="h-6 w-full flex items-center px-2 space-x-1 border-b" style={{ backgroundColor: colors['--bg-header'], borderColor: colors['--border-dark'] }}>
          <div className="w-2 h-2 rounded-full" style={{ backgroundColor: colors['--accent-primary'] }} />
          <div className="w-2 h-2 rounded-full opacity-50" style={{ backgroundColor: colors['--accent-secondary'] }} />
          <div className="ml-auto w-10 h-1.5 rounded opacity-20" style={{ backgroundColor: colors['--text-muted'] }} />
        </div>
        
        {/* Body Preview */}
        <div className="flex-1 flex p-2 space-x-2">
          {/* Sidebar */}
          <div className="w-1/4 h-full rounded border" style={{ backgroundColor: colors['--bg-panel'], borderColor: colors['--border-light'] }}>
            <div className="mt-2 mx-1 h-1.5 w-3/4 rounded opacity-40" style={{ backgroundColor: colors['--text-muted'] }} />
            <div className="mt-1 mx-1 h-1.5 w-1/2 rounded opacity-30" style={{ backgroundColor: colors['--text-muted'] }} />
          </div>
          {/* Content */}
          <div className="flex-1 h-full p-2">
             <div className="w-2/3 h-2 rounded mb-2" style={{ backgroundColor: colors['--accent-primary'] }} />
             <div className="w-full h-1.5 rounded mb-1 opacity-80" style={{ backgroundColor: colors['--text-main'] }} />
             <div className="w-5/6 h-1.5 rounded mb-1 opacity-80" style={{ backgroundColor: colors['--text-main'] }} />
             <div className="w-4/5 h-1.5 rounded mb-1 opacity-80" style={{ backgroundColor: colors['--text-main'] }} />
          </div>
        </div>
      </div>

      {/* Footer / Info */}
      <div className="p-3 bg-win-panel border-t border-win-borderLight flex items-center justify-between">
        <span className={`text-sm font-semibold truncate ${isActive ? 'text-win-accent' : 'text-win-text'}`}>
          {theme.name}
        </span>
        <button 
          onClick={onExport}
          className="p-1.5 rounded hover:bg-win-header text-win-muted hover:text-win-accent transition-colors"
          title="Export Theme JSON"
        >
          <Download size={14} />
        </button>
      </div>

      {isActive && (
        <div className="absolute top-2 right-2 bg-win-accent text-win-bg p-1 rounded-full shadow-lg">
          <Check size={12} strokeWidth={4} />
        </div>
      )}
    </div>
  );
};

const ThemeGallery: React.FC<ThemeGalleryProps> = ({ isOpen, onClose, themes, currentTheme, onApplyTheme, onImportTheme }) => {
  const fileInputRef = useRef<HTMLInputElement>(null);
  const [error, setError] = useState<string | null>(null);

  if (!isOpen) return null;

  const handleExport = (e: React.MouseEvent, theme: Theme) => {
    e.stopPropagation();
    try {
      const dataStr = "data:text/json;charset=utf-8," + encodeURIComponent(JSON.stringify(theme, null, 2));
      const downloadAnchorNode = document.createElement('a');
      downloadAnchorNode.setAttribute("href", dataStr);
      downloadAnchorNode.setAttribute("download", `${theme.name.replace(/\s+/g, '_').toLowerCase()}.theme.json`);
      document.body.appendChild(downloadAnchorNode);
      downloadAnchorNode.click();
      downloadAnchorNode.remove();
    } catch (err) {
      setError("Failed to export theme.");
    }
  };

  const handleImportClick = () => {
    fileInputRef.current?.click();
  };

  const handleFileChange = (event: React.ChangeEvent<HTMLInputElement>) => {
    setError(null);
    const file = event.target.files?.[0];
    if (!file) return;

    const reader = new FileReader();
    reader.onload = (e) => {
      try {
        const json = JSON.parse(e.target.result as string);
        
        // Basic Validation
        if (!json.name || !json.colors) {
          throw new Error("Invalid theme format: Missing name or colors.");
        }
        
        // Ensure ID is unique-ish
        const newTheme: Theme = {
          ...json,
          id: json.id || `custom-${Date.now()}`
        };

        onImportTheme(newTheme);
      } catch (err) {
        console.error(err);
        setError("Error parsing theme file. Ensure it is valid JSON.");
      }
    };
    reader.readAsText(file);
    // Reset input
    event.target.value = '';
  };

  return (
    <div className="fixed inset-0 z-50 flex items-center justify-center bg-black/80 backdrop-blur-sm animate-fade-in p-4">
      <div className="bg-win-bg w-full max-w-4xl h-[80vh] flex flex-col rounded-lg border-2 border-win-borderLight shadow-2xl overflow-hidden relative">
        
        {/* Header */}
        <div className="bg-win-header border-b border-win-borderDark p-4 flex items-center justify-between select-none">
          <div className="flex items-center space-x-3">
             <div className="p-2 bg-win-accent/20 rounded text-win-accent">
               <Upload size={20} className="rotate-180" /> 
             </div>
             <div>
               <h2 className="text-xl font-bold text-win-text">Theme Gallery</h2>
               <p className="text-xs text-win-muted uppercase tracking-wider">Select or Import Appearance Profile</p>
             </div>
          </div>
          <button 
            onClick={onClose}
            className="p-2 hover:bg-red-500/20 hover:text-red-400 rounded transition-colors text-win-muted"
          >
            <X size={24} />
          </button>
        </div>

        {/* Toolbar */}
        <div className="p-4 bg-win-panel border-b border-win-borderLight flex items-center justify-between">
          <div className="text-sm text-win-muted">
            Viewing <span className="text-win-accent font-mono">{themes.length}</span> installed themes
          </div>
          
          <div className="flex items-center space-x-4">
            {error && (
              <div className="flex items-center text-red-400 text-sm animate-pulse">
                <AlertCircle size={16} className="mr-2" />
                {error}
              </div>
            )}
            <input 
              type="file" 
              ref={fileInputRef} 
              className="hidden" 
              accept=".json" 
              onChange={handleFileChange}
            />
            <button 
              onClick={handleImportClick}
              className="flex items-center space-x-2 px-4 py-2 bg-win-accent/10 hover:bg-win-accent/20 text-win-accent border border-win-accent/30 rounded transition-all text-sm font-semibold uppercase tracking-wide"
            >
              <Upload size={16} />
              <span>Import Theme</span>
            </button>
          </div>
        </div>

        {/* Grid */}
        <div className="flex-1 overflow-y-auto p-6 bg-win-bg custom-scrollbar">
          <div className="grid grid-cols-1 sm:grid-cols-2 md:grid-cols-3 gap-6">
            {themes.map((theme) => (
              <ThemePreview 
                key={theme.id} 
                theme={theme} 
                isActive={currentTheme.id === theme.id}
                onClick={() => onApplyTheme(theme)}
                onExport={(e) => handleExport(e, theme)}
              />
            ))}
          </div>
        </div>

      </div>
    </div>
  );
};

export default ThemeGallery;
