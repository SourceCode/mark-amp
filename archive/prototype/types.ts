export interface FileNode {
  id: string;
  name: string;
  type: 'file' | 'folder';
  content?: string; // Only for files
  children?: FileNode[]; // Only for folders
  isOpen?: boolean; // For folder toggle state
}

export interface Theme {
  id: string;
  name: string;
  colors: {
    '--bg-app': string;
    '--bg-panel': string;
    '--bg-header': string;
    '--bg-input': string;
    '--text-main': string;
    '--text-muted': string;
    '--accent-primary': string;
    '--accent-secondary': string;
    '--border-light': string;
    '--border-dark': string;
  };
}

export enum ViewMode {
  EDITOR = 'EDITOR',
  PREVIEW = 'PREVIEW',
  SPLIT = 'SPLIT',
}
